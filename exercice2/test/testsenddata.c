#define FTP_PWD 1
#define FTP_LPWD 2
#define FTP_LS 3
#define FTP_LLS 4
#define FTP_CD 5
#define FTP_LCD 6
#define FTP_FILE_STOC 7
#define FTP_FILE_CTOS 8
#define FTP_BYE 9
#define FTP_RESP_LS 10
#define FTP_RESP_PWD 11
#define FTP_RESP_CD 12
#define FTP_ERROR 13
#define FTP_SUCCESS 14
#define FTP_HELP 15
#define TCP_SIZE 536
#define BUFF_SIZE 1200


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
//#include <sys/socket.h>
#include <signal.h>
#include <ctype.h>
//#include <arpa/inet.h>
//#include <netdb.h>
#include <fcntl.h>
#include <getopt.h>
struct controlMessage {
  int type;
  int argLength;
  char *arg;
};

void func_cd(char *dirPath);
void func_exec(char* cmd);
void get_response(int type, int sd, struct controlMessage *m);
void get_file(int sd, struct controlMessage *m, struct sockaddr_in sockaddrS);
void send_file(int sd, struct controlMessage *m, struct sockaddr_in sockaddrS);
void send_message(int sd, struct controlMessage *m);
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
void send_message(int sd, struct controlMessage *m)
{
    write(sd,m,sizeof(m));
}
void get_response(int type, int sd, struct controlMessage *m)
{
    send_message(sd,m);
    int* typeMessage=NULL;
    read(sd,typeMessage,sizeof(int));
    if(*typeMessage != type || *typeMessage != FTP_ERROR)
    {
        fprintf( stderr, "Error of transmission" );
        return;
    }
    int* length=NULL;
    read(sd,length,sizeof(int));
    char* buff=NULL;
    read(sd,buff,*length*sizeof(char));
    fprintf( stdout, "%s", buff);

}



void clear_message(struct controlMessage *m)
{
    m->type=0;
    m->argLength=0;
    m->arg=NULL;
}

void create_message(char *cmd, struct controlMessage *m)
{
    if(strcmp(cmd,"pwd") == 0 && strlen(cmd) == 3)
    {
        m->type = FTP_PWD;
    }
    else if(strcmp(cmd,"lpwd") == 0 && strlen(cmd) == 4)
    {
        m->type = FTP_LPWD;
    }
    else if (strcmp(cmd,"ls") == 0 && strlen(cmd) == 2)
    {
        m->type = FTP_LS;
    }
    else if (strcmp(cmd,"lls") == 0 && strlen(cmd) == 3)
    {
        m->type = FTP_LLS;
    }
    else if (strncmp(cmd,"cd ",3) == 0)
    {
        m->type = FTP_CD;
        char *tmp=cmd;
        tmp = tmp+3*sizeof(char);
        while(tmp[0] == ' ')
        {
            tmp = tmp + sizeof(char);
        }
        m->argLength = strlen(tmp);
        m->arg=tmp;
    }
    else if (strncmp(cmd,"lcd ",4) == 0)
    {
        m->type = FTP_LCD;
        char *tmp=cmd;
        tmp = tmp+4*sizeof(char);
        while(tmp[0] == ' ')
        {
            tmp = tmp + 1;
        }
        m->argLength = strlen(tmp);
        m->arg=tmp;
    }
    else if (strncmp(cmd,"get ",4) == 0)
    {
        m->type = FTP_FILE_STOC;
        char *tmp=cmd;
        tmp = tmp+4*sizeof(char);
        while(tmp[0] == ' ')
        {
            tmp = tmp + 1;
        }
        m->argLength = strlen(tmp);
        m->arg=tmp;
    }
    else if (strncmp(cmd,"put ",4) == 0)
    {
        m->type = FTP_FILE_CTOS;
        char *tmp=cmd;
        tmp = tmp+4*sizeof(char);
        while(tmp[0] == ' ')
        {
            tmp = tmp + 1;
        }
        m->argLength = strlen(tmp);
        m->arg=tmp;
    }
    else if (strcmp(cmd,"bye") == 0 && strlen(cmd) == 3)
    {
        m->type = FTP_BYE;
    }
    else
    {
        m->type= FTP_HELP;
    }
}


void parse(int argc, char* argv[], char** serverAddr)
{
    int optch;
    extern int opterr;

    char format[] = "h";

    while ((optch = getopt(argc, argv, format)) != -1)
    switch (optch)
    {
        case 'h':
            printf ("This program takes one argument : The name of the host where the server program is running \n");
            break;
    }
    if(argc != 2)
    {
        printf ("The number of arguments is incorrect\n");
        printf ("This program takes one argument : The name of the host where the server program is running \n");
        exit(0);
    }
    *serverAddr = (char*)argv[1];

}

void display_help()
{

}

void main()
{
    struct controlMessage m;
    clear_message(&m);

    create_message("ls",&m);
    fprintf( stdout, "%i \n", m.type  );
    fprintf( stdout, "%i \n", m.argLength  );
    fprintf( stdout, "%s \n", m.arg  );

    create_message("lls",&m);
    fprintf( stdout, "%i \n", m.type  );
    fprintf( stdout, "%i \n", m.argLength  );
    fprintf( stdout, "%s \n", m.arg  );

    create_message("pwd",&m);
    fprintf( stdout, "%i \n", m.type  );
    fprintf( stdout, "%i \n", m.argLength  );
    fprintf( stdout, "%s \n", m.arg  );

    create_message("lpwd",&m);
    fprintf( stdout, "%i \n", m.type  );
    fprintf( stdout, "%i \n", m.argLength  );
    fprintf( stdout, "%s \n", m.arg  );

    create_message("cd llooll",&m);
    fprintf( stdout, "%i \n", m.type  );
    fprintf( stdout, "%i \n", m.argLength  );
    fprintf( stdout, "%s \n", m.arg  );

    create_message("lcd llooll",&m);
    fprintf( stdout, "%i \n", m.type  );
    fprintf( stdout, "%i \n", m.argLength  );
    fprintf( stdout, "%s \n", m.arg  );

    create_message("get menu",&m);
    fprintf( stdout, "%i \n", m.type  );
    fprintf( stdout, "%i \n", m.argLength  );
    fprintf( stdout, "%s \n", m.arg  );

    create_message("put   menusdhreherhrherhrehrhrehrheherhreerrheehehrehhdzdzdzdzdzdzdzdzfaefezgrehehreherherhreherhreerehreherhrheherhrherhreherherhre",&m);
    fprintf( stdout, "%i \n", m.type  );
    fprintf( stdout, "%i \n", m.argLength  );
    fprintf( stdout, "%s \n", m.arg  );

    create_message("bye",&m);
    fprintf( stdout, "%i \n", m.type  );
    fprintf( stdout, "%i \n", m.argLength  );
    fprintf( stdout, "%s \n", m.arg  );

    create_message("byeeeee",&m);
    fprintf( stdout, "%i \n", m.type  );
    fprintf( stdout, "%i \n", m.argLength  );
    fprintf( stdout, "%s \n", m.arg  );

    create_message("error!",&m);
    fprintf( stdout, "%i \n", m.type  );
    fprintf( stdout, "%i \n", m.argLength  );
    fprintf( stdout, "%s \n", m.arg  );

    exit(0);

}
