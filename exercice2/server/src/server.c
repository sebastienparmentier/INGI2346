/* ===================================
* FTP basic program
* (C) 2013 M. Biset, S. Parmentier
* =================================== */

#include "inclSocket.h"
#include "utilityFunctions.h"

#define PORT 7000
#define BACKLOG 5

void clear_message(struct controlMessage *m)
{
    m->type=0;
    m->argLength=0;
    m->arg=NULL;
}
void recieve_file(int sdf, char* name)
{
    struct controlMessage m;
    clear_message(&m);
    m.type=FTP_SUCCESS;
    send_message(sdf,&m);
    
    FILE *fd = fopen(m.arg, "wb");
    if(fd == NULL)
    {
        printf("The file %s can't be opened.\n", m.arg);
        free(m.arg);
        close(sdf);             
    }
    
    int buffer[(TCP_SIZE)/4]; 
    bzero(buffer, TCP_SIZE); 
    int blockSize=0;
    bzero(buffer, TCP_SIZE); 
	while((blockSize = recv(sdf, buffer, TCP_SIZE, 0)) > 0)
    {
        int nb = fwrite(buffer, sizeof(char), blockSize, fd);
	    if(nb < blockSize)
        {
            printf("File write failed.\n");
        }
        bzero(buffer, TCP_SIZE);
	    if (blockSize == 0 || blockSize != TCP_SIZE)
        {
            break;
        }
	}
	if(blockSize < 0)
    {
        if (errno == EAGAIN)
        {
            printf("recv() timed out.\n");
        }
        else 
        {
            fprintf(stderr, "recv() failed due to errno = %d\n", errno);
        }
	}
	fclose(fd);

}
void transfer_file(int sdf, char* name)
{
    struct controlMessage m;
    clear_message(&m);
    m.type=FTP_SUCCESS;
    send_message(sdf,&m);
    
    FILE *fd = fopen(m.arg, "rb");
    int buffer[(TCP_SIZE)/4]; 
    bzero(buffer, TCP_SIZE); 
    int blockSize; 
    while((blockSize = fread(buffer, sizeof(int), (TCP_SIZE)/4, fd)) > 0)
    {
        int s = send(sdf, buffer, blockSize, 0);
        bzero(buffer, (TCP_SIZE)/4);
        if(s < 0)
        {
            fprintf(stderr, "The transmission of the file %s has failed", m.arg);
            break;
        }
        else if(s == 0)
        {
            fprintf(stderr, "The transmission of the file %s has failed because the connection has been lost", m.arg);
            break;
        }
	}
	fclose(fd);
}



void transfer_and_recieve_files(int sd)
{
    if(listen(sd,BACKLOG) == -1) 
    {
        perror("The file sender/reciever can't listen on port %i");
        close(sd);
        exit(1);
    }
    
    fprintf(stderr,"The file sender/reciever is listening on port %i ...",PORT);
    
     /* Wait a connection, and obtain a new socket file descriptor for single connection */
    int sdf;
    socklen_t sin_size; 
    struct sockaddr_in sockaddrC;
    if ((sdf = accept(sd, (struct sockaddr *)&sockaddrC, &sin_size)) < 0) 
    {
        perror("The file sender/reciever can't accept a connection\n");
        close(sd);
        exit(1);  
    }
    
    int* typeMessage=NULL;
    read(sdf,typeMessage,sizeof(int));
    if(*typeMessage != FTP_CTOS|| *typeMessage != FTP_STOC)
    {   
        fprintf( stderr, "unauthorized connection to the FTP server" );
        close(sdf);
        return;
    }
    int* length=NULL;
    read(sdf,length,sizeof(int));
    char* name=NULL;
    read(sdf,name,*length*sizeof(char));
    if(*typeMessage == FTP_CTOS)
        recieve_file(sdf,name);      
    else
        transfer_file(sdf,name);
        
    close(sdf);
    

}
void func_send(char* cmd)
{



}

bool recieve_message(int sd)
{
    struct controlMessage mess;
    clear_message(&mess);
    read(sd,&mess.type,sizeof(int));
    read(sd,&mess.argLength,sizeof(int));
    read(sd,&mess.arg,(mess.argLength)*sizeof(char));
    if(mess.type == FTP_PWD)
        func_send("pwd");
    else if(mess.type == FTP_LS)
        func_send("ls");
    else if(mess.type == FTP_CD)
    {
        get_response(FTP_LS,sd,&mess);
        func_cd(mess.arg);
    }
    else if(mess.type == FTP_BYE)
    {
        return false;
    }
    return true;
}


int main(int argc, char* argv[]) 
{
    int sd;
    fprintf(stderr,"1");
    if ( ( sd = socket (PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("The server socket can't be created.\n");
        exit(1);
    }
    fprintf(stderr,"2");
    struct sockaddr_in sockaddrS;

    /* the structure "sockaddrS" with the local address of the
       server */
  
	memset((char *) &sockaddrS, 0, sizeof(sockaddrS));
    sockaddrS.sin_family     = AF_INET;
    sockaddrS.sin_addr.s_addr = INADDR_ANY;
    sockaddrS.sin_port       = htons(PORT);
    if( bind(sd, (struct sockaddr*)&sockaddrS, sizeof(struct sockaddr)) == -1 ) 
    {
        perror("The server socket can't be created.\n");
        close(sd);
        exit(1);
    } 
    fprintf(stderr,"3");
    if(listen(sd,BACKLOG) == -1) 
    {
        perror("The server can't listen on port %i");
        close(sd);
        exit(1);
    }
    fprintf(stderr,"4");
    fprintf(stderr,"FTP server listening on port %i ...",PORT);
    int newsd;
     /* Wait a connection, and obtain a new socket file despriptor for single connection */
    socklen_t sin_size; 
    struct sockaddr_in sockaddrC;
    if ((newsd = accept(sd, (struct sockaddr *)&sockaddrC, &sin_size)) < 0) 
    {
        perror("The server can't accept a connection\n");
        close(sd);
        exit(1);  
    }
    pid_t pid = fork();
    if(pid == 0)
    {/*Child procedure => waiting file commands*/
        close(newsd);
        transfer_and_recieve_files(sd);
    }
    /*parent procedure => waiting usual commands*/
    
    bool contin = true;

    while(contin)
    {   
        contin = recieve_message(newsd);
    }
    close(newsd);
    close(sd);
    /*wait the file sender*/
    int status=0;
    waitpid(pid,&status,0);
    exit(0);
}

