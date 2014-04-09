#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include "libvmi_rpc_client.h"
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <stdarg.h>



int mit_main (struct rvmi_instance *prvmi);
int mit_main (struct rvmi_instance *prvmi)
{
    vmi_instance_t vmi;
    unsigned char *memory = NULL;
    uint32_t offset;
    addr_t list_head = 0, current_list_entry = 0, next_list_entry = 0;
    addr_t current_process = 0;
    addr_t tmp_next = 0;
    char *procname = NULL;
    uint32_t pid = 0;
    unsigned long tasks_offset, pid_offset, name_offset;
    status_t status;
    int i;

    char *name = "VM2-ovs";

    /* initialize the libvmi library */
    rvmi_init(prvmi, VMI_AUTO | VMI_INIT_COMPLETE, name);

    /* init the offset values */

    tasks_offset = rvmi_get_offset(prvmi, "linux_tasks");
    name_offset = rvmi_get_offset(prvmi, "linux_name");
    pid_offset = rvmi_get_offset(prvmi, "linux_pid");



    /* pause the vm for consistent memory access
    if (vmi_pause_vm(vmi) != VMI_SUCCESS) {
        printf("Failed to pause VM\n");
        goto error_exit;
    } */// if



for(i=0;i<500;i++)
{

    current_process = rvmi_translate_ksym2v(prvmi, "init_task");


    /* walk the task list */
    list_head = current_process + tasks_offset;
    current_list_entry = list_head;

    status = rvmi_read_addr_va(prvmi, current_list_entry, 0, &next_list_entry);

    //printf("Next list entry is at: %lx\n", next_list_entry);

    do {
        /* Note: the task_struct that we are looking at has a lot of
         * information.  However, the process name and id are burried
         * nice and deep.  Instead of doing something sane like mapping
         * this data to a task_struct, I'm just jumping to the location
         * with the info that I want.  This helps to make the example
         * code cleaner, if not more fragile.  In a real app, you'd
         * want to do this a little more robust :-)  See
         * include/linux/sched.h for mode details */

        /* NOTE: _EPROCESS.UniqueProcessId is a really VOID*, but is never > 32 bits,
         * so this is safe enough for x64 Windows for example purposes */
        rvmi_read_32_va(prvmi, current_process + pid_offset, 0, &pid);

        procname = rvmi_read_str_va(prvmi, current_process + name_offset, 0);

        if (!procname) {
            printf("Failed to find procname\n");
            goto error_exit;
        }

        /* print out the process name */
        //printf("[%5d] %s \n", pid, procname);
        if (procname) {
            free(procname);
            procname = NULL;
        }

        current_list_entry = next_list_entry;
        current_process = current_list_entry - tasks_offset;

        /* follow the next pointer */

        status = rvmi_read_addr_va(prvmi, current_list_entry, 0, &next_list_entry);


    } while (next_list_entry != list_head);



    //printf("i=%d\n",i);
}
    error_exit: if (procname)
        free(procname);
    /* resume the vm
    vmi_resume_vm(vmi);*/

    /* cleanup any memory associated with the LibVMI instance */
    rvmi_destroy(prvmi);

    return 0;
}



int main()
{
    int cfd;
    int recbytes;
    int sin_size;
    char buffer[]="hello epoll\n";
    struct sockaddr_in s_add,c_add;
    unsigned short portnum=7001;

    struct rvmi_instance rvmi, *prvmi;

    //printf("Hello,welcome to client !\r\n");

    cfd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == cfd)
    {
        printf("socket fail ! \r\n");
        return -1;
    }
    //printf("socket ok !\r\n");

    bzero(&s_add,sizeof(struct sockaddr_in));
    s_add.sin_family=AF_INET;
    s_add.sin_addr.s_addr= inet_addr("127.0.0.1");
    s_add.sin_port=htons(portnum);
    //printf("s_addr = %#x ,port : %#x\r\n",s_add.sin_addr.s_addr,s_add.sin_port);

    if(-1 == connect(cfd,(struct sockaddr *)(&s_add), sizeof(struct sockaddr_in)))
    {
        printf("connect fail !\r\n");
        return -1;
    }
    //printf("connect ok !\r\n");

    rvmi.cfd= cfd;
    rvmi.s_add = (struct sockaddr *)(&s_add);
    prvmi = (struct rvmi_instance  *)&rvmi;

    mit_main(prvmi);
    close(cfd);
    return 0;
}
