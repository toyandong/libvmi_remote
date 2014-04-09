#ifndef LIBVMI_RPC_CLIENT_H
#define LIBVMI_RPC_CLIENT_H

/*
 *
 *
 * 1  vmi_clear_event
 * 2  vmi_convert_str_encoding
 * 3  vmi_destroy
 * 4  vmi_dtb_to_pid
 * 5  vmi_events_listen
 * 6  vmi_free_unicode_str
 * 7  vmi_get_access_mode
 * 8  vmi_get_address_width
 * 9  vmi_get_mem_event
 * 10  vmi_get_memsize
 * 11  vmi_get_name
 * 12  vmi_get_num_vcpus
 * 13  vmi_get_offset
 * 14  vmi_get_ostype
 * 15  vmi_get_page_mode
 * 16  vmi_get_reg_event
 * 17  vmi_get_singlestep_event
 * 18  vmi_get_vcpureg
 * 19  vmi_get_vmid
 * 20  vmi_get_winver
 * 21  vmi_get_winver_manual
 * 22  vmi_get_winver_str
 * 23  vmi_init
 * 24  vmi_init_complete
 * 25  vmi_init_complete_custom
 * 26  vmi_init_custom
 * 27  vmi_pagetable_lookup
 * 28  vmi_pause_vm
 * 29  vmi_pid_to_dtb
 * 30  vmi_pidcache_add
 * 31  vmi_pidcache_flush
 * 32  vmi_print_hex
 * 33  vmi_print_hex_ksym
 * 34  vmi_print_hex_pa
 * 35  vmi_print_hex_va
 * 36  vmi_read_16_ksym
 * 37  vmi_read_16_pa
 * 38  vmi_read_16_va
 * 39  vmi_read_32_ksym
 * 40  vmi_read_32_pa
 * 41  vmi_read_32_va
 * 42  vmi_read_64_ksym
 * 43  vmi_read_64_pa
 * 44  vmi_read_64_va
 * 45  vmi_read_8_ksym
 * 46  vmi_read_8_pa
 * 47  vmi_read_8_va
 * 48  vmi_read_addr_ksym
 * 49  vmi_read_addr_pa
 * 50  vmi_read_addr_va
 * 51  vmi_read_ksym
 * 52  vmi_read_pa
 * 53  vmi_read_str_ksym
 * 54  vmi_read_str_pa
 * 55  vmi_read_str_va
 * 56  vmi_read_unicode_str_va
 * 57  vmi_read_va
 * 58  vmi_register_event
 * 59  vmi_resume_vm
 * 60  vmi_rvacache_add
 * 61  vmi_rvacache_flush
 * 62  vmi_set_vcpureg
 * 63  vmi_shutdown_single_step
 * 64  vmi_stop_single_step_vcpu
 * 65  vmi_symcache_add
 * 66  vmi_symcache_flush
 * 67  vmi_translate_ksym2v
 * 68  vmi_translate_kv2p
 * 69  vmi_translate_sym2v
 * 70  vmi_translate_uv2p
 * 71  vmi_translate_v2sym
 * 72  vmi_v2pcache_add
 * 73  vmi_v2pcache_flush
 * 74  vmi_write_16_ksym
 * 75  vmi_write_16_pa
 * 76  vmi_write_16_va
 * 77  vmi_write_32_ksym
 * 78  vmi_write_32_pa
 * 79  vmi_write_32_va
 * 80  vmi_write_64_ksym
 * 81  vmi_write_64_pa
 * 82  vmi_write_64_va
 * 83  vmi_write_8_ksym
 * 84  vmi_write_8_pa
 * 85  vmi_write_8_va
 * 86  vmi_write_ksym
 * 87  vmi_write_pa
 * 88  vmi_write_va
 */

#include <libvmi/libvmi.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <stdarg.h>


#define VMIHandleID_t int


struct rvmi_instance
{
    int cfd;
    struct sockaddr * s_add;
    VMIHandleID_t vmi;
};


typedef struct rvmi_instance rvmi_instance_t;

VMIHandleID_t rvmi_init(rvmi_instance_t *rvmi, uint32_t flags,char *name);

status_t rvmi_destroy(rvmi_instance_t *rvmi);

unsigned long rvmi_get_offset(rvmi_instance_t *rvmi,char *offset_name);

addr_t rvmi_translate_ksym2v (rvmi_instance_t *rvmi, char *symbol);

status_t rvmi_read_addr_va(rvmi_instance_t *rvmi, addr_t vaddr,int pid,addr_t *value);

status_t rvmi_read_32_va(rvmi_instance_t *rvmi, addr_t vaddr,int pid,uint32_t * value);


char *rvmi_read_str_va(rvmi_instance_t *rvmi, addr_t vaddr,int pid);

#endif


