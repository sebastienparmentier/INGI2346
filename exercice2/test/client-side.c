#define FTP_PWD 1
#define FTP_LPWD 2
#define FTP_LS 3
#define FTP_LLS 4
#define FTP_CD 5
#define FTP_LCD 6
#define FTP_STOC 7
#define FTP_CTOS 8
#define FTP_BYE 9
#define FTP_GET 10
#define FTP_PUT 11
#define FTP_RESP_LS 12
#define FTP_RESP_PWD 13
#define FTP_ERROR_DIR 14
#define FTP_ERROR_LS 15
#define FTP_ERROR_FILE 16
#define FTP_ERROR 17
#define FTP_SUCCESS 0
#define FTP_PROG 42424242
#define FTP_VERS 1
#define BUFF_SIZE 1200
#define PROMPT '#'
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
//#include <netinet/in.h>
//#include <sys/wait.h>
#include <signal.h>
#include <ctype.h>
//#include <arpa/inet.h>
//#include <netdb.h>
#include <fcntl.h>

void display_help()
{
    printf ("The number of arguments is incorrect\n");
    printf ("This program takes one argument : The name of the host where the server program is running \n");
}

void func_cd(char *dirPath)
{
    chdir(dirPath);
}

void func_exec(char* cmd)
{
    FILE *f = popen( cmd, "r" );
    if ( f == 0 )
    {
        fprintf( stderr, "We can't execute this command\n" );
        return;
    }
    char str[256];
    while( fgets( str, 256,  f )!= NULL )
        fprintf( stdout, "%s", str  );

    pclose( f );
}

void quit_server(CLIENT *clnt)
{
    quit((void *)NULL, clnt);
}

void func_serv_cd(CLIENT *clnt, struct controlMessage *mess)
{
    int* state;
    state = serv_cd(mess,clnt);
    if(resp == NULL)
    {
        fprintf( stdout, "Command transmission error\n", resp->arg );
    }
    else if(*state == FTP_SUCCESS)
    {
        fprintf( stdout, "Server is at %s\n", mess->arg  );
    }
    else if(*state == FTP_ERROR_DIR)
    {
        fprintf( stderr, "The directory %s doesn't exist\n", mess->arg  );
    }
    else
    {
        fprintf( stderr, "Transmission error\n");
    }

}

void get_pwd(CLIENT clnt)
{
    struct controlMessage *resp;
    resp = serv_pwd(mess,clnt);
    if(resp == NULL)
        fprintf( stdout, "Command transmission error\n", resp->arg );
    else
        fprintf( stdout, "%s\n", resp->arg );
}

void get_ls(CLIENT clnt)
{
    struct controlMessage *resp;
    resp = serv_pwd(mess,clnt);
    fprintf( stdout, "%s\n", resp->arg );
}

void parse(int argc, char* argv[])
{
    if(argc != 2)
    {
        display_help;
        exit(0);
    }

}

main(int argc, char **argv)
{

    parse(argc,argv);
    char *serverName = (char*)argv[1];
    CLIENT *clnt;
    clnt = clnt_create(serverName, FTP_PROG, FTP_VERS, "tcp");

    if (clnt == (CLIENT *) NULL  )
    {
    /*
    * Couldn't establish connection with server.
    */
        clnt_pcreateerror(serverName);
        exit(1);
    }

    bool cont = true;
    while(cont)
    {
        printf("%c ",PROMPT);
        char* ptr = NULL;
        char* cmd = NULL;
        for(int size=256;ptr==NULL;size=size*2)
        {
            if ( (cmd=realloc(cmd, size * sizeof(*cmd))) ==NULL)
            {
                perror("memory error");
                clnt_destroy( clnt );
                exit(1);
            }
            fgets(cmd, size, stdin);
            ptr = memchr(cmd, '\n', size);
        }
        *ptr = '\0';
        struct controlMessage mess;
        clear_message(&mess);
        create_message(cmd, &mess);
        if(mess.type == FTP_PWD)
            get_pwd(clnt);
        else if(mess.type == FTP_LPWD)
            func_exec("pwd");
        else if(mess.type == FTP_LS)
            get_ls(clnt);
        else if(mess.type == FTP_LLS)
            func_exec("ls");
        else if(mess.type == FTP_CD)
            func_serv_cd(clnt, &mess);
        else if(mess.type == FTP_LCD)
            func_cd(&mess);
        else if(mess.type == FTP_STOC)
            get_file(clnt, &mess);
        else if(mess.type == FTP_CTOS)
            send_file(clnt, &mess);
        else if(mess.type == FTP_BYE)
        {
            cont=false;
            quit_server(clnt);
        }
        else
            display_help();
        free(cmd);

    }
    clnt_destroy( clnt );
    exit(0);
}
