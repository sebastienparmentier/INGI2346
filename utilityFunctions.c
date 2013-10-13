struct controlMessage {
  int type;
  int argLength
  char *arg;
};
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
    char* str[256];
    while( fgets( str, 256,  f )== NULL ) 
        fprintf( stdout, "%s", str  );
    
    pclose( f );
}
void send(int sd, struct controlMessage *m)
{
    write(sd,&m,sizeof(m));
}
void get_cd(struct controlMessage *m)
{
    send(sd,m);
    /*read something*/

}