#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
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
void func_cd(char *dirPath)
{
    chdir(dirPath);
}
int main(int argc, char* argv[])
{
    func_exec("dir");
    func_cd("..");
    func_exec("dir");
    func_cd("..");
    func_exec("dir");
    func_cd("..");
    func_exec("dir");
    func_cd("..");
    func_exec("dir");
        func_cd("..");
    func_exec("dir");
        func_cd("..");
    func_exec("dir");
        func_cd("..");
    func_exec("dir");
        func_cd("..");
    func_exec("dir");
        func_cd("..");
    func_exec("dir");
        func_cd("..");
    func_exec("dir");
    exit(0);

}
