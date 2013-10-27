#define FTP_PWD 1
#define FTP_LPWD 2
#define FTP_LS 3
#define FTP_LLS 4
#define FTP_CD 5
#define FTP_LCD 6
#define FTP_FILE_STOC 7
#define FTP_FILE_CTOS 8
#define FTP_BYE 9
#define FTP_GET 10
#define FTP_PUT 11
#define FTP_RESP_LS 12
#define FTP_RESP_PWD 13
#define FTP_ERROR_DIR 14
#define FTP_ERROR_LS 15
#define FTP_ERROR_FILE 16
#define FTP_ERROR 17
#define FTP_TRANSFER 18
#define FTP_TRANSFER_END 19
#define FTP_SUCCESS 0
#define FTP_PROG 42424242
#define FTP_VERS 1
#define BUFF_SIZE 4096
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

struct controlMessage {
    int type;
    int argLength;
    char *arg;
};

struct requestTransferMessage {
    int type;
    long fileOffset;
    int argLength;
    char *fileName;
};

struct transferMessage {
    int type;
    long fileOffset;
    int argLength;
    char *fileName;
    char *buffer; //size supposed to be BUFFER_SIZE
};

void clear_message(struct controlMessage *m)
{
    m->type=0;
    m->argLength=0;
    m->arg=NULL;
}

void clear_transferMessage(struct transferMessage *m)
{
    m->type=0;
    m->fileOffset =0;
    m->argLength=0;
    m->fileName=NULL;
    m->buffer=NULL;
}
void clear_requestMessage(struct requestTransferMessage *m)
{
    m->type=0;
    m->fileOffset =0;
    m->argLength=0;
    m->fileName=NULL;
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
        m->type= FTP_ERROR;
    }
}


void display_help()
{
    printf ("The number of arguments is incorrect\n");
    printf ("This program takes one argument : The name of the host where the server program is running \n");
}

void func_cd(char *dirPath)
{
    int ret = chdir(dirPath);
    if(ret != 0)
    {
        fprintf(stdout, "We can't change to this directory : %s\n", dirPath);
    }
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

void func_serv_cd(CLIENT *clnt, struct controlMessage *mess)
{
    int* state;
    state = serv_cd(mess,clnt);
    if(state == NULL)
    {
        fprintf( stdout, "Command transmission error\n");
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
    if(resp == (int*) NULL)
        fprintf( stdout, "Command transmission error\n", resp->arg );
    else if(resp->type != FTP_SUCCESS)

    else
        fprintf( stdout, "%s\n", resp->arg );
}

void get_ls(CLIENT clnt)
{
    struct transferMessage *resp;
    struct requestTransferMessage mess;
    clear_requestMessage(mess);
    mess.type=FTP_LS;
    mess.fileOffset=0;
    resp = serv_ls(mess,clnt);
    if(resp == NULL)
    {
        fprintf( stdout, "Error of transmission\n");
        return;
    }
    while(resp != NULL && resp->type == FTP_TRANSFER)
    {
        fprintf( stdout, "%s", resp->buffer);
        mess.fileOffset = resp->fileOffset;
        resp = serv_ls(mess,clnt);

    }
    if(resp == NULL)
    {
        fprintf( stdout, "Error of transmission\n");
        return;
    }
    else if (resp->type != FTP_TRANSFER_END)
    {
        fprintf( stdout, "Error in the execution of ls command\n");
        return;
    }
    fprintf( stdout, "%s", resp->buffer);
    fprintf( stdout, "\n");

}

void get_file(CLIENT clnt, struct controlMessage *m)
{
    struct transferMessage *resp;
    struct requestTransferMessage mess;
    clear_requestMessage(mess);
    mess.type=m->type;
    mess.arglength=m->argLength;
    mess.fileName = m->arg;
    FILE *fd = fopen(m->arg, "wb");
    if(fd == NULL)
    {
        printf("The file %s can't be opened.\n", m->arg);
        return;
    }

    resp = serv_download(mess,clnt);
    int nb = BUFF_SIZE;
    while(resp != NULL && resp->type == FTP_TRANSFER && nb==BUFF_SIZE)
    {
        nb = fwrite(resp->buffer,  sizeof(char), BUFFER_SIZE, fd);
        mess.fileOffset = resp->fileOffset;
        resp = serv_ls(mess,clnt);

    }
    if(resp == NULL)
    {
        fprintf( stdout, "Error of transmission\n");
        fclose(fd);
        return;
    }
    else if (resp->type == FTP_FILE_ERROR)
    {
        fprintf( stdout, "The file %s couldn't be opened on the server\n", m->arg);
        fclose(fd);
        return;
    }
    else if (resp->type != FTP_TRANSFER_END)
    {
        fprintf( stdout, "Error in downloading the file %s\n", m->arg);
        fclose(fd);
        return;
    }
    fwrite(resp->buffer,  sizeof(char), BUFFER_SIZE, fd);
    fclose(fd);

}

void send_file(CLIENT clnt, struct controlMessage *m)
{
    struct transferMessage mess;
    struct requestMessage *resp = NULL;
    clear_transferMessage(mess);
    mess.type=m->type;
    mess.arglength=m->argLength;
    mess.fileName = m->arg;
    char* buff[BUFF_SIZE];
    mess.buffer = buff;
    FILE *fd = fopen(m->arg, "rb");
    if(fd == NULL)
    {
        printf("The file %s can't be opened.\n", m->arg);
        return;
    }
    int nb;
    nb = fread(resp->buffer,  sizeof(char), BUFFER_SIZE, fd);
    for(;nb>0; mess.fileOffset += BUFF_SIZE*(sizeof(char)))
    {
        for(int i = 0;(resp == NULL || resp->type != SUCCESS )&& i<5 ;i++)
            resp = serv_upload(mess,clnt);

        if(resp == NULL || resp->type != SUCCESS)
        {
            fprintf( stdout, "Error in the transfer of the file\n");
            fclose(fd);
            return;
        }

        nb = fread(resp->buffer,  sizeof(char), BUFFER_SIZE, fd);
    }
    if(feof(fd))
    {
        for(int i = 0;(resp == NULL || resp->type != SUCCESS )&& i<5 ;i++)
            resp = serv_upload(mess,clnt);

        if(resp == NULL || resp->type != SUCCESS)
        {
            fprintf( stdout, "Error in the transfer of the file\n");
        }
    }
    else
    {
        fprintf( stdout, "Error in the transfer of the file\n");
    }
    fclose(fd);

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

    if (clnt == (CLIENT*) NULL  )
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
            func_cd(mess.arg);
        else if(mess.type == FTP_FILE_STOC)
            get_file(clnt, &mess);
        else if(mess.type == FTP_FILE_CTOS)
            send_file(clnt, &mess);
        else if(mess.type == FTP_BYE)
            cont=false;
        else
            display_help();
        free(cmd);

    }
    clnt_destroy( clnt );
    exit(0);
}
