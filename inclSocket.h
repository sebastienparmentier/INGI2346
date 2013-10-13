#ifndef __INCLSOCKET_H__
#define __INCLSOCKET_H__
#define FTP_PWD 1
#define FTP_LPWD 2
#define FTP_LS 3
#define FTP_LLS 4
#define FTP_CD 5
#define FTP_LCD 6
#define FTP_STOC 7
#define FTP_CTOS 8
#define FTP_BYE 9
#define FTP_RESP_LS 10
#define FTP_RESP_PWD 11
#define FTP_RESP_CD 12
#define FTP_ERROR 13

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <signal.h>
#include <ctype.h>          
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#endif

