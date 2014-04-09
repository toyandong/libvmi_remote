/* The LibVMI Library is an introspection library that simplifies access to
 * memory in a target virtual machine or in a file containing a dump of
 * a system's physical memory.  LibVMI is based on the XenAccess Library.
 *
 * Copyright 2011 Sandia Corporation. Under the terms of Contract
 * DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government
 * retains certain rights in this software.
 *
 * Author: Bryan D. Payne (bdpayne@acm.org)
 *
 * This file is part of LibVMI.
 *
 * LibVMI is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * LibVMI is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with LibVMI.  If not, see <http://www.gnu.org/licenses/>.
 */

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

    vmi_init(&vmi, VMI_AUTO | VMI_INIT_COMPLETE, argv[1]);

    for(i=0;i<1000000;i++)
    {
         //vmi_init(&vmi, VMI_AUTO | VMI_INIT_COMPLETE, argv[1]);
        //current_process = vmi_translate_ksym2v(vmi, "init_task");
        //status = vmi_read_addr_va(vmi, current_list_entry, 0, &next_list_entry);
        vmi_read_32_va(vmi, 3358775776, 0, &pid);
        vmi_read_32_va(vmi, 3358775776, 0, &pid);
        //printf("pid %d\n",pid);
        // procname = vmi_read_str_va(vmi, current_process + name_offset, 0);
         //vmi_destroy(vmi);
    }

    vmi_destroy(vmi);

    return 0;
}




int main (int argc, char **argv)
{

    mit_main (argc, argv);
	return 1;
}










