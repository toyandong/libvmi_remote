
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

    return 0;
}




int main (int argc, char **argv)
{

    mit_main (argc, argv);
	return 1;
}









