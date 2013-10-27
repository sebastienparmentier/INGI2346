#include < stdio.h >
#include "msg.h"
/*  Print a message on the console */


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

int *serv_pwd_2(mess, rqstp)
struct controlMessage *mess;
struct svc_req *rqstp;
{
    struct controlMessage res_pwd;
    clear_message(&res_pwd);
    FILE *f = popen( "pwd", "r" );
    if ( f == 0 )
    {
        res_pwd.type=FTP_ERROR;
        return(&res_pwd) ;
    }
    char str[BUFF_SIZE];
    int nb = fgets( str, BUFF_SIZE,  f );
    res_pwd.arg = str;
    res_pwd.argLength = nb;

    pclose( f );
    return(&res_pwd) ;
}

/*
* dir_proc.c: remote readdir
* implementation
*/

#include <dirent.h>
#include "dir.h" /* Created by rpcgen */

extern int errno;

extern char *malloc();
extern char *strdup();

readdir_res *
readdir_1(nametype *dirname, struct svc_req *req)

{
  DIR *dirp;
  struct dirent *d;
  namelist nl;
  namelist *nlp;

  static readdir_res res; /* must be static! */

  /* Open directory */
  dirp = opendir(*dirname);

 if (dirp == (DIR *)NULL) {
    res.errno = errno;
   return (&res);
  }

  /* Free previous result */
  xdr_free(xdr_readdir_res, &res);

  /*
   * Collect directory entries.
   * Memory allocated here is free by
   * xdr_free the next time readdir_1
   * is called
   */

   nlp = &res.readdir_res_u.list;
   while (d = readdir(dirp)) {
     nl = *nlp = (namenode *)
     malloc(sizeof(namenode));
     if (nl == (namenode *) NULL) {
       res.errno = EAGAIN;
       closedir(dirp);
       return(&res);
      }
    nl->name = strdup(d->d_name);
    nlp = &nl->next;
  }

  *nlp = (namelist)NULL;

  /* Return the result */
  res.errno = 0;
  closedir(dirp);
  return (&res);
}
