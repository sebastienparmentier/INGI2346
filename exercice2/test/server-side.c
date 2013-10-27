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
//#include <rpc/rpc.h>
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

int *serv_cd_1(mess, rqstp)
struct controlMessage *mess;
struct svc_req *rqstp;
{
    static int res_cd=FTP_SUCCESS;

    int ret = chdir(mess->arg);
    if(ret != 0)
        res_cd = FTP_ERROR_DIR;
    return(&res_cd);
}

struct controlMessage *serv_pwd_2(mess, rqstp)
struct controlMessage *mess;
struct svc_req *rqstp;
{
    static struct controlMessage res_pwd;
    clear_message(&res_pwd);
    res_pwd.type=FTP_SUCCESS;
    FILE *f = popen( "pwd", "r" );
    if ( f == 0 )
    {
        res_pwd.type=FTP_ERROR;
        return(&res_pwd) ;
    }
    char str[BUFF_SIZE];
    char *ptr = fgets( str, BUFF_SIZE,  f );
    if(ptr ==NULL)
    {
        res_pwd.type=FTP_ERROR;
        pclose(f);
        return(&res_pwd) ;
    }
    res_pwd.arg = str;
    res_pwd.argLength = strlen(str);
    pclose( f );
    return(&res_pwd) ;
}

struct transferMessage *serv_ls_3(mess, rqstp)
struct requestTransferMessage *mess;
struct svc_req *rqstp;
{
    static struct transferMessage res_ls;
    clear_transferMessage(&res_ls);
    res_ls.type = FTP_TRANSFER;
    FILE *f = popen( "ls", "r" );
    if( f == 0 )
    {
        res_ls.type=FTP_ERROR_LS;
        return(&res_ls) ;
    }
    int ret = fseek(f, (mess->fileOffset), SEEK_SET);
    if(ret !=0)
    {
        res_ls.type=FTP_ERROR;
        pclose( f );
        return(&res_ls);
    }
    char str[BUFF_SIZE];
    char *ptr = fgets( str, BUFF_SIZE,  f );
    if(ptr ==NULL)
    {
        res_ls.type=FTP_ERROR;
        pclose(f);
        return(&res_ls) ;
    }
    res_ls.buffer = str;

    if(feof(f))
    {
        res_ls.type = FTP_TRANSFER_END;
        pclose( f );
        return(&res_ls);
    }

    pclose( f );
    return(&res_ls);
}

struct transferMessage *serv_download_4(mess, rqstp)
struct requestTransferMessage *mess;
struct svc_req *rqstp;
{
    static struct transferMessage res_dwld;
    clear_transferMessage(&res_dwld);
    res_dwld.type = FTP_TRANSFER;
    FILE *f = fopen(mess->fileName, "rb");
    if(f == NULL)
    {
        res_dwld.type=FTP_ERROR_FILE;
        return(&res_dwld) ;
    }
    int nb;
    char buff[BUFF_SIZE];
    nb = fread(buff,  sizeof(char), BUFF_SIZE, f);
    if(feof(f))
    {
        res_dwld.type=FTP_TRANSFER_END;
        fclose( f );
        return(&res_dwld);
    }
    else if(nb < 0)
    {
        res_dwld.type=FTP_ERROR;
        fclose( f );
        return(&res_dwld);
    }
    res_dwld.buffer = buff;

    fclose(f);
    return(&res_dwld);
}

struct requestTransferMessage *serv_upload_5(mess, rqstp)
struct transferMessage *mess;
struct svc_req *rqstp;
{
    static struct requestTransferMessage res_upld;
    clear_requestMessage(&res_upld);
    res_upld.fileOffset = mess->fileOffset;
    res_upld.type = FTP_SUCCESS;
    FILE *f = fopen(mess->fileName, "wb");

    if(f == NULL)
    {
        res_upld.type=FTP_ERROR_FILE;
        return(&res_upld) ;
    }
    int ret;
    ret=fseek(f, mess->fileOffset, SEEK_SET);
    int nb;
    char buff[BUFF_SIZE];
    nb = fwrite(mess->buffer,  sizeof(char), BUFF_SIZE, f);
    if(nb < 0)
    {
        res_upld.type=FTP_ERROR;
    }
    fclose(f);
    return(&res_upld) ;
}
