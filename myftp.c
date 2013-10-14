/* ===================================
* FTP basic program
* (C) 2013 M. Biset, S. Parmentier
* =================================== */

#include "inclSocket.h"
#include "utilityFunctions.h"

#define PORT 7000
#define PROMPT '#'

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
        char *tmp;
        tmp = strtok(cmd, " ");
        while(tmp[0] == ' ')
        {
            tmp = tmp + 1;
        }
        m->argLength = strlen(tmp);
        m->arg=tmp;
    }
    else if (strncmp(cmd,"lcd ",4) == 0)
    {
        m->type = FTP_LCD;
        char *tmp;
        tmp = strtok(cmd, " ");
        while(tmp[0] == ' ')
        {
            tmp = tmp + 1;
        }
        m->argLength = strlen(tmp);
        m->arg=tmp;
    }
    else if (strncmp(cmd,"get ",4) == 0)
    {
        m->type = FTP_GET;
        char *tmp;
        tmp = strtok(cmd, " ");
        while(tmp[0] == ' ')
        {
            tmp = tmp + 1;
        }
        m->argLength = strlen(tmp);
        m->arg=tmp;
    }
    else if (strncmp(cmd,"put ",4) == 0)
    {
        m->type = FTP_PUT;
        char *tmp;
        tmp = strtok(cmd, " ");
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
            printf ("This program takes one argument : The name of the host where the server program is running \n");
            break;
    }
    if(argc != 2)
    {
        printf ("The number of arguments is incorrect\n");
        printf ("This program takes one argument : The name of the host where the server program is running \n");
        exit(0);  
    }
    *serverAddr = (char*)&argv[1];

}

void get_file(int sd, struct controlMessage *mess)
{
}

void func_exec(char str[])
{
}

void get_response(int type, int sd, struct controlMessage *m)
{
}

void func_cd(char *arg)
{
}

void send_file(int sd, struct controlMessage *m, struct sockaddr_in sockaddrS)
{
}


void ourSend(int sd, struct controlMessage *m)
{
}

void display_help()
{

}

int main(int argc, char* argv[]) 
{
    int sd;
    char* serverAddr = NULL;
    parse(argc,argv, &serverAddr);
    if ( ( sd = socket (PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("The socket can't be created.\n");
        exit(1);
    }
    struct sockaddr_in sockaddrS;

    /* fill in the structure "sockaddrS" with the address of the
     * server that we want to connect with */
  
		memset((char *) &sockaddrS, 0, sizeof(sockaddrS));
    /*bzero((char *) &sockaddrS , sizeof(sockaddrS));*/
    sockaddrS.sin_family     = AF_INET;
    sockaddrS.sin_addr.s_addr = inet_addr(serverAddr);
    sockaddrS.sin_port       = htons(PORT);
    if (connect(sd , (struct sockaddr *) &sockaddrS , sizeof ( sockaddrS )) < 0)
    {
        perror("We can't connect to the server");
        close(sd);
        exit(1);
    }

    bool b = true;
    while(b)
    {   
        printf("%s ",PROMPT);
        char* ptr = NULL;
        char* cmd = NULL;
        for(int size=256;ptr==NULL;size=size*2)
        {
            if ( (cmd=realloc(cmd, size * sizeof(*cmd))) ==NULL)
            {
                perror("memory error ");
                close(sd);
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
            get_response(FTP_PWD,sd,&mess);
        else if(mess.type == FTP_LPWD)
            func_exec("pwd");
        else if(mess.type == FTP_LS)
            get_response(FTP_LS,sd,&mess);
        else if(mess.type == FTP_LLS)
            func_exec("ls");
        else if(mess.type == FTP_CD)
            get_response(FTP_CD,sd,&mess);
        else if(mess.type == FTP_LCD)
            func_cd(mess.arg);
        else if(mess.type == FTP_STOC)
            get_file(sd,&mess);
        else if(mess.type == FTP_CTOS)
            send_file(sd,&mess,sockaddrS);
        else if(mess.type == FTP_BYE){
            b=false;
            ourSend(sd,&mess);}
        else
            display_help();
        free(cmd);

    }
    close(sd);
    exit(0);
}

