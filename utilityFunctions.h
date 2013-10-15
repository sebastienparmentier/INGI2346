#ifndef __UTILITY_H__
#define __UTILITY_H__

struct controlMessage {
  int type;
  int argLength;
  char *arg;
};

void func_cd(char *dirPath);
void func_exec(char* cmd);
void ourSend(int sd, struct controlMessage *m);
void get_response(int type, int sd, struct controlMessage *m);
void send_file(int sd, struct controlMessage *m, struct sockaddr_in sockaddrS);

#endif