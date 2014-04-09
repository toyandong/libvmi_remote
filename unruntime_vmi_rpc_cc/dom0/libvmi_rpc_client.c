#include "libvmi_rpc_client.h"

//char * vmi_rpc_send(int cfd, struct sockaddr * s_add,char *VMName,char* funName,char *funArgsType,...);

VMIHandleID_t rvmi_init(rvmi_instance_t *rvmi, uint32_t flags,char *name)
{
    char sendString[100];
    int num;
    char returnVal[100];
    int rvmiID;

    num = sprintf(sendString, "%d %u %s", 23,flags,name);

    if( (num=send(rvmi->cfd, sendString, num, 0)) < 0)
    {
        perror("send出错！");
    }

    num=recv(rvmi->cfd, returnVal, 100, 0);
    rvmiID = atoi(returnVal);
    //printf("recv %s", returnVal);
    rvmi->vmi = rvmiID;
    return rvmiID;

}

status_t rvmi_destroy(rvmi_instance_t *rvmi)
{
    char sendString[100];
    int num;
    char returnVal[100];

    num = sprintf(sendString, "%d %d", 3, rvmi->vmi);

    if( (num=send(rvmi->cfd, sendString, num, 0)) < 0)
    {
        perror("send出错！");
    }

    num=recv(rvmi->cfd, returnVal, 100, 0);
    return 1;
}

unsigned long rvmi_get_offset(rvmi_instance_t *rvmi,char *offset_name)
{
    char sendString[100];
    int num;
    char returnVal[100];

    num = sprintf(sendString, "%d %d %s", 13, rvmi->vmi, offset_name);

    if( (num=send(rvmi->cfd, sendString, num, 0)) < 0)
    {
        perror("send出错！");
    }

    num=recv(rvmi->cfd, returnVal, 100, 0);
    //printf("recv %s", returnVal);
    return (unsigned long)atol(returnVal);
}

addr_t rvmi_translate_ksym2v (rvmi_instance_t *rvmi, char *symbol)
{
    char sendString[100];
    int num;
    char returnVal[100];

    num = sprintf(sendString, "%d %d %s", 67, rvmi->vmi, symbol);

    if( (num=send(rvmi->cfd, sendString, num, 0)) < 0)
    {
        perror("send出错！");
    }

    num=recv(rvmi->cfd, returnVal, 100, 0);
    //printf("recv %s", returnVal);
    return (addr_t)atol(returnVal);
}

status_t rvmi_read_addr_va(rvmi_instance_t *rvmi, addr_t vaddr,int pid,addr_t *value)
{
    char sendString[100];
    int num;
    char returnVal[100];

    num = sprintf(sendString, "%d %d %lu %d", 50, rvmi->vmi, vaddr, pid);

    if( (num=send(rvmi->cfd, sendString, num, 0)) < 0)
    {
        perror("send出错！");
    }

    num=recv(rvmi->cfd, returnVal, 100, 0);
    //printf("recv %s", returnVal);
    *value = (addr_t)atol(returnVal);
    return 1;
}

status_t rvmi_read_32_va(rvmi_instance_t *rvmi, addr_t vaddr,int pid,uint32_t * value)
{
    char sendString[100];
    int num;
    char returnVal[100];

    num = sprintf(sendString, "%d %d %lu %d", 40, rvmi->vmi, vaddr, pid);

    if( (num=send(rvmi->cfd, sendString, num, 0)) < 0)
    {
        perror("send出错！");
    }

    num=recv(rvmi->cfd, returnVal, 100, 0);
    //printf("recv %s", returnVal);
    *value = (uint32_t)atoi(returnVal);
    return 1;

}

status_t rvmi_read_64_va(rvmi_instance_t *rvmi, addr_t vaddr,int pid,uint64_t * value)
{
    char sendString[100];
    int num;
    char returnVal[100];

    num = sprintf(sendString, "%d %d %lu %d", 44, rvmi->vmi, vaddr, pid);

    if( (num=send(rvmi->cfd, sendString, num, 0)) < 0)
    {
        perror("send出错！");
    }

    num=recv(rvmi->cfd, returnVal, 100, 0);
    //printf("recv %s", returnVal);
    *value = (uint64_t)atol(returnVal);
}

status_t rvmi_read_16_va(rvmi_instance_t *rvmi, addr_t vaddr,int pid,uint16_t * value)
{
    char sendString[100];
    int num;
    char returnVal[100];

    num = sprintf(sendString, "%d %d %lu %d", 38, rvmi->vmi, vaddr, pid);

    if( (num=send(rvmi->cfd, sendString, num, 0)) < 0)
    {
        perror("send出错！");
    }

    num=recv(rvmi->cfd, returnVal, 100, 0);
    //printf("recv %s", returnVal);
    *value = (uint16_t)atoi(returnVal);
}


char *rvmi_read_str_va(rvmi_instance_t *rvmi, addr_t vaddr,int pid)
{
    char sendString[100];
    int num;
    char *returnVal = malloc(100*sizeof(char));

    num = sprintf(sendString, "%d %d %lu %d", 55, rvmi->vmi, vaddr, pid);

    if( (num=send(rvmi->cfd, sendString, num, 0)) < 0)
    {
        perror("send出错！");
    }

    num=recv(rvmi->cfd, returnVal, 100, 0);
    returnVal[num]='\0';
    //printf("recv %s", returnVal);
    return returnVal;
}




