#include <libvmi/libvmi.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <stdio.h>

int mit_main (int argc, char **argv);
int mit_main (int argc, char **argv)
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

    /* initialize the libvmi library */
    vmi_init(&vmi, VMI_AUTO | VMI_INIT_COMPLETE, argv[1]);

    tasks_offset = vmi_get_offset(vmi, "linux_tasks");
    name_offset = vmi_get_offset(vmi, "linux_name");
    pid_offset = vmi_get_offset(vmi, "linux_pid");




for(i=0;i<1000;i++)
{

    current_process = vmi_translate_ksym2v(vmi, "init_task");

    /* walk the task list */
    list_head = current_process + tasks_offset;
    current_list_entry = list_head;

    status = vmi_read_addr_va(vmi, current_list_entry, 0, &next_list_entry);

    do {

        vmi_read_32_va(vmi, current_process + pid_offset, 0, &pid);

        procname = vmi_read_str_va(vmi, current_process + name_offset, 0);

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

        status = vmi_read_addr_va(vmi, current_list_entry, 0, &next_list_entry);
        if (status == VMI_FAILURE) {
            printf("Failed to read next pointer in loop at %lx\n", current_list_entry);
            goto error_exit;
        }

    } while (next_list_entry != list_head);


    //printf("i=%d\n",i);
}
    error_exit: if (procname)
        free(procname);
    vmi_destroy(vmi);

    return 0;
}




int main (int argc, char **argv)
{

    mit_main (argc, argv);
	return 1;
}










