#ifndef  INCLUDE_RPC_VMI_H
#define  INCLUDE_RPC_VMI_H
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

struct VMNode
{
    //ip e.g "127.0.1.1"
    struct in_addr ip;
    //port e.g 8001
    unsigned short port;
    //VMName "VM1-ovs"
    char name[16];
};


char * vmi_rpc_send(int cfd, struct sockaddr * s_add,char *VMName,char* funName,char *funArgTypes,...);
char * vmi_rpc_init(int cfd, struct sockaddr * s_add,char *VMName);
char * vmi_rpc_destroy(int cfd, struct sockaddr * s_add,char *VMName);

#endif
