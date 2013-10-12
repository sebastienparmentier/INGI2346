/* ===================================
* FTP basic program
* (C) 2013 M. Biset, S. Parmentier
* =================================== */

#include "inclSocket.h"

#define PORT 7000


struct controlMessage {
  int type;
  int argLength
  char *arg;
};

void init_message(struct controlMessage *m)
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
    else if (strcmp(cad,"bye") == 0 && strlen(cad) == 3)
    {
        m->type = FTP_BYE;
    }
    else
    {
        m->type= FTP_HELP;
    }
}

func_lpwd()
{

}

func_cd(char *orden)
{
    chdir(orden);
}
void parse(int argc, char* argv)
{
    int optch;
    extern int opterr = 1;
 
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

}
main(int argc, char* argv) 
{
    int sd;
    parse(argc,argv);

    sd=socket(AF_INET,SOCK_STREAM,0); 
    if(sd<0)
    {
        printf("The socket can't be created.\n");
        exit(1);
    }
    struct sockaddr_in addr; 
    bzero(&addr,sizeof(struct sockaddr_in)); 
    addr.sin_family=AF_INET; 
    addr.sin_port=htons(PORT); 
    inet_aton("192.168.196.129",&addr.sin_addr); 
  
    for(;cont;) 
    {   

        connect(sd,(struct sockaddr *)&addr,sizeof(addr));

    

    }
    close(sd);
    exit(0);
}

