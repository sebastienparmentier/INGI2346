#include "inclSocket.h"
#include "utilityFunctions.h"

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

void send_file(int sd, struct controlMessage *m, struct sockaddr_in sockaddrS)
{
    FILE *fd = fopen(m->arg, "r");
    if(fd == NULL)
    {
        printf("We can't find the file : %s \n", m->arg);
    }
    fseek(fd,0,SEEK_END);
    unsigned long size = ftell(fd);
    fclose(fd);
    int pid = fork();
    if(pid!=0)
        return;

    /*Child process*/
    close(sd);
    int sdf;
    if ( ( sdf = socket (PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("The socket used to transfer the file can't be created.\n");
        free(m->arg);
        exit(1);
    }

    if (connect(sdf , (struct sockaddr *) &sockaddrS , sizeof ( sockaddrS )) < 0)
    {
        perror("We can't establish a connection to transfer the file");
        free(m->arg);
        close(sdf);
        exit(1);
    }
    send_message(sdf,m);
    write(sdf,&size,sizeof(size));

    int* typeMessage=NULL;
    read(sdf,typeMessage,sizeof(int));
    if(*typeMessage != FTP_SUCCESS || *typeMessage != FTP_ERROR)
    {
        fprintf( stderr, "Error of transmission" );
        return;
    }
    int* length=NULL;
    read(sdf,length,sizeof(int));
    char* buff=NULL;
    read(sdf,buff,*length*sizeof(char));
    if(*typeMessage == FTP_ERROR)
    {
        fprintf(stdout, "%s", buff);
        return;
    }

    fd = fopen(m->arg, "rb");
    int buffer[(TCP_SIZE)/4];
    bzero(buffer, TCP_SIZE);
    int blockSize;
    while((blockSize = fread(buffer, sizeof(int), (TCP_SIZE)/4, fd)) > 0)
    {
        int s = send(sdf, buffer, blockSize, 0);
        bzero(buffer, (TCP_SIZE)/4);
        if(s < 0)
        {
            fprintf(stderr, "The transmission of the file %s has failed", m->arg);
            break;
        }
        else if(s == 0)
        {
            fprintf(stderr, "The transmission of the file %s has failed because the connection has been lost", m->arg);
            break;
        }
	}
    free(m->arg);
    close(sdf);
    exit(0);

}
void get_file(int sd, struct controlMessage *m, struct sockaddr_in sockaddrS)
{
    int pid = fork();
    if(pid!=0)
        return;
    /*Child process*/
    close(sd);
    int sdf;
    if ( ( sdf = socket (PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("The socket used to transfer the file can't be created.\n");
        free(m->arg);
        exit(1);
    }

    if (connect(sdf , (struct sockaddr *) &sockaddrS , sizeof ( sockaddrS )) < 0)
    {
        perror("We can't establish a connection to transfer the file");
        free(m->arg);
        close(sdf);
        exit(1);
    }
    send_message(sdf,m);
    int* typeMessage=NULL;
    read(sdf,typeMessage,sizeof(int));
    if(*typeMessage != FTP_SUCCESS || *typeMessage != FTP_ERROR)
    {
        fprintf( stderr, "Error of transmission" );
        return;
    }
    int* length=NULL;
    read(sdf,length,sizeof(int));
    char* buff=NULL;
    read(sdf,buff,*length*sizeof(char));
    if(*typeMessage == FTP_ERROR)
    {
        fprintf(stdout, "%s", buff);
        free(m->arg);
        close(sdf);

    }
    FILE *fd = fopen(m->arg, "wb");
    if(fd == NULL)
    {
        printf("The file %s can't be opened.\n", m->arg);
        free(m->arg);
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


}

