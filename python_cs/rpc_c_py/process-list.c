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

void *mit_main(void * vm_addr);
char * vmi_rpc_send(int cfd, struct sockaddr * s_add,char *VMName,char* funName,char *funArgTypes,...);
char * vmi_rpc_init(int cfd, struct sockaddr * s_add,char *VMName);
char * vmi_rpc_destroy(int cfd, struct sockaddr * s_add,char *VMName);


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


/*监控远端虚拟机*/
void *mit_main(void * vm_info)
{
    int i;
    struct VMNode * vm=(struct VMNode *)vm_info;
    printf("==============%s=======================\n", vm->name);
    //printf("hello fun:%s, %u, %s\n",inet_ntoa(vm->ip),vm->port, vm->name);
    /*建立socket链接*/
    int cfd;
    char buffer[1024]="hello c socket livmi";
    struct sockaddr_in s_add;

    cfd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == cfd)
    {
        return (void *)-1;
    }

    bzero(&s_add,sizeof(struct sockaddr_in));
    s_add.sin_family=AF_INET;
    s_add.sin_addr.s_addr=vm->ip.s_addr;
    s_add.sin_port=htons(vm->port);
    //printf("s_addr = %#x ,port : %#x\r\n",s_add.sin_addr.s_addr,s_add.sin_port);

    if(-1 == connect(cfd,(struct sockaddr *)(&s_add), sizeof(struct sockaddr)))
    {
        return (void *)-1;
    }

    /*建立vmi会话，在远端dom0上保存这个会话*/
    vmi_rpc_init(cfd,(struct sockaddr *)(&s_add),vm->name);
    /*进行监控，即调用一系列libvmi函数*/
    struct sockaddr * s_addr_arg=(struct sockaddr *)(&s_add);
    //printf("renturn int :%d\n", atoi(vmi_rpc_send(cfd,(struct sockaddr *)(&s_add),vm->name,"get_offset","s","linux_tasks") ));
    int tasksOffset,nameOffset,pidOffset,init_task_va, processes,current_process;
    int pid;
    char * procname=NULL;
    tasksOffset =atoi(vmi_rpc_send(cfd,s_addr_arg,vm->name,"get_offset","s","linux_tasks"));
    nameOffset = atoi(vmi_rpc_send(cfd,s_addr_arg,vm->name,"get_offset","s","linux_name"));
    pidOffset = atoi(vmi_rpc_send(cfd,s_addr_arg,vm->name,"get_offset","s","linux_pid"));
    init_task_va = atoi(vmi_rpc_send(cfd,s_addr_arg,vm->name,"translate_ksym2v","s","init_task"));
//for(i=0;i<1000;i++)
//{
    processes = atol(vmi_rpc_send(cfd,s_addr_arg,vm->name,"read_addr_va","li",init_task_va+tasksOffset,0));
    current_process = processes;
    while(1)
    {
        pid=atoi(vmi_rpc_send(cfd,s_addr_arg,vm->name,"read_32_va","li",current_process+pidOffset-tasksOffset,0));
        procname=vmi_rpc_send(cfd,s_addr_arg,vm->name,"read_str_va","li",current_process+nameOffset-tasksOffset,0);
        printf("[%5d] %s\n",pid, procname);
        current_process =atol(vmi_rpc_send(cfd,s_addr_arg,vm->name,"read_addr_va","li",current_process,0));
        if(current_process == processes)
            break;
        if(procname!=NULL)
        {
            free(procname);
            procname=NULL;
        }

    }
  //  printf("i=%d\n",i);
//}
    /*销毁vmi会话*/
    vmi_rpc_destroy(cfd,(struct sockaddr *)(&s_add),vm->name);
    /*断开socket链接*/
    close(cfd);
}



/**
 *为每个VM建立监控
 *
 **/
int main()
{

    /*temp code*/
    int i=0;
    struct VMNode VMList[4];

    VMList[0].port=8003;
    VMList[0].ip.s_addr=inet_addr("127.0.0.1");
    memcpy(VMList[0].name, "VM1-ovs", sizeof("VM1-ovs"));

    VMList[1].port=8004;
    VMList[1].ip.s_addr=inet_addr("127.0.0.1");
    memcpy(VMList[1].name, "VM1-ovs", sizeof("VM1-ovs"));

    VMList[2].port=8003;
    VMList[2].ip.s_addr=inet_addr("127.0.0.1");
    memcpy(VMList[2].name, "VM2-ovs", sizeof("VM2-ovs"));


    VMList[3].port=8004;
    VMList[3].ip.s_addr=inet_addr("127.0.0.1");
    memcpy(VMList[3].name, "VM2-ovs", sizeof("VM2-ovs"));

    /*end temp code*/
    pthread_t pthread_id_1;
    pthread_t pthread_id_2;
    pthread_t pthread_id_3;
    pthread_t pthread_id_4;

    pthread_create(&pthread_id_1, NULL, &mit_main, &VMList[0]);
    pthread_create(&pthread_id_2, NULL, &mit_main, &VMList[1]);
    pthread_create(&pthread_id_3, NULL, &mit_main, &VMList[2]);
    pthread_create(&pthread_id_4, NULL, &mit_main, &VMList[3]);

    pthread_join(pthread_id_1, NULL);
    pthread_join(pthread_id_2, NULL);
    pthread_join(pthread_id_3, NULL);
    pthread_join(pthread_id_4, NULL);



}