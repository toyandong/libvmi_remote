
#include "rpc_vmi.h"



char * vmi_rpc_init(int cfd, struct sockaddr * s_add,char *VMName)
{
    return vmi_rpc_send(cfd,s_add,VMName,"pyvmi.init","s",VMName);
}

char * vmi_rpc_destroy(int cfd, struct sockaddr * s_add,char *VMName)
{
    return vmi_rpc_send(cfd,s_add,VMName,"pyvmi.destroy","s",VMName);
}


char * vmi_rpc_send(int cfd, struct sockaddr * s_add,char *VMName,char* funName,char *funArgsType,...)
{
    int i;
    int arg_int;
    uint64_t arg_long;
    char *arg_string;

    char funArgsString[1024];
    int postion=0;
    int num;
    va_list ap;
    va_start(ap,funArgsType);
    for(i=0;i<10;i++)
    {
        //printf("Type: %d\n",funArgsType[i]);
        if('\n'==funArgsType[i] || 0==funArgsType[i])
            break;
        if('i' == funArgsType[i])
        {
            arg_int=va_arg(ap,int);
            num=sprintf(funArgsString+postion,"%d",arg_int);
            postion +=num;
        }else if('l' == funArgsType[i])
        {
             arg_long=va_arg(ap,uint64_t);
             num=sprintf(funArgsString+postion,"%lu",arg_long);
             postion +=num;
        }else if('s' == funArgsType[i])
        {
             arg_string=va_arg(ap,char *);
             num=sprintf(funArgsString+postion,"%s",arg_string);
             postion +=num;
        }else
        {
            printf("ERROR:should not be here!\n");
            break;
        }

        if('\n'!=funArgsType[i+1] && 0!=funArgsType[i+1])
        {
            num=sprintf(funArgsString+postion," ",arg_string);
            postion +=num;
        }

    }
    //printf("%s--%d\n",funArgsString,postion);
    va_end(ap);

    char buffer[1024];
    char *returnVal=NULL;
    int addr_len = sizeof(struct sockaddr_in);
    num=sprintf(buffer,"%s %s %s %s",VMName, funName,funArgsType,funArgsString);
    //printf("%s---%d\n",buffer,num);
    //return NULL;
    if( (num=sendto(cfd, buffer, num, 0, s_add, addr_len)) < 0)
    {
        perror("send出错！");
    }
    //printf("send %d\n",num);
    returnVal = (char *)malloc(1024);
    num=recvfrom(cfd, returnVal, 1024, 0, s_add, &addr_len);
    //printf("recv, %d--%s\n",num,returnVal);
    return returnVal;
}



