#include "libvmi_rpc_server.h"

void (*vmiFunArr[])()={
      vmi_clear_event
    , vmi_convert_str_encoding
    , vmi_destroy
    , vmi_dtb_to_pid
    , vmi_events_listen
    , vmi_free_unicode_str
    , vmi_get_access_mode
    , vmi_get_address_width
    , vmi_get_mem_event
    , vmi_get_memsize
    , vmi_get_name
    , vmi_get_num_vcpus
    , vmi_get_offset
    , vmi_get_ostype
    , vmi_get_page_mode
    , vmi_get_reg_event
    , vmi_get_singlestep_event
    , vmi_get_vcpureg
    , vmi_get_vmid
    , vmi_get_winver
    , vmi_get_winver_manual
    , vmi_get_winver_str
    , vmi_init
    , vmi_init_complete
    , vmi_init_complete_custom
    , vmi_init_custom
    , vmi_pagetable_lookup
    , vmi_pause_vm
    , vmi_pid_to_dtb
    , vmi_pidcache_add
    , vmi_pidcache_flush
    , vmi_print_hex
    , vmi_print_hex_ksym
    , vmi_print_hex_pa
    , vmi_print_hex_va
    , vmi_read_16_ksym
    , vmi_read_16_pa
    , vmi_read_16_va
    , vmi_read_32_ksym
    , vmi_read_32_pa
    , vmi_read_32_va
    , vmi_read_64_ksym
    , vmi_read_64_pa
    , vmi_read_64_va
    , vmi_read_8_ksym
    , vmi_read_8_pa
    , vmi_read_8_va
    , vmi_read_addr_ksym
    , vmi_read_addr_pa
    , vmi_read_addr_va
    , vmi_read_ksym
    , vmi_read_pa
    , vmi_read_str_ksym
    , vmi_read_str_pa
    , vmi_read_str_va
    , vmi_read_unicode_str_va
    , vmi_read_va
    , vmi_register_event
    , vmi_resume_vm
    , vmi_rvacache_add
    , vmi_rvacache_flush
    , vmi_set_vcpureg
    , vmi_shutdown_single_step
    , vmi_stop_single_step_vcpu
    , vmi_symcache_add
    , vmi_symcache_flush
    , vmi_translate_ksym2v
    , vmi_translate_kv2p
    , vmi_translate_sym2v
    , vmi_translate_uv2p
    , vmi_translate_v2sym
    , vmi_v2pcache_add
    , vmi_v2pcache_flush
    , vmi_write_16_ksym
    , vmi_write_16_pa
    , vmi_write_16_va
    , vmi_write_32_ksym
    , vmi_write_32_pa
    , vmi_write_32_va
    , vmi_write_64_ksym
    , vmi_write_64_pa
    , vmi_write_64_va
    , vmi_write_8_ksym
    , vmi_write_8_pa
    , vmi_write_8_va
    , vmi_write_ksym
    , vmi_write_pa
    , vmi_write_va
};

vmi_instance_t vmiArr[MAX_DOMU_PER_MACHINE];
int vmiFlagArr[MAX_DOMU_PER_MACHINE]={0};



/*根据data，执行vmi函数，并发送返回值*/
int rvmi_handle_data(int fd, char *buf)
{
    int vmiFunNum;
    int i=0;
    char delims[] = " ";
    char *vmiFunArg[5] = {NULL,NULL,NULL,NULL};
    char returnBuf[100];
    char * returnVal =NULL;

    vmiFunArg[i] = strtok(buf, delims );
    while( vmiFunArg[i] != NULL )
    {
        i++;
        vmiFunArg[i] = strtok( NULL, delims );
    }


    vmiFunNum = atoi(vmiFunArg[0]);
    //printf("vmifun %d\n",vmiFunNum);
    switch(vmiFunNum)
    {
        case 1: break;
        case 2: break;
        case 3:
        {
            int vmiID=atoi(vmiFunArg[1]);
            sprintf(returnBuf, "%d\0", vmi_destroy(vmiArr[vmiID]) );
            vmiFlagArr[vmiID]=0;
            returnVal = returnBuf;
        }
        break;
        case 13:
        {
            sprintf(returnBuf, "%lu\0", vmi_get_offset(vmiArr[atoi(vmiFunArg[1])], vmiFunArg[2]) );
            returnVal = returnBuf;
        }

        break;
        case 23:
        {
            for(i=0;i<MAX_DOMU_PER_MACHINE;i++)
            {
                if(0 == vmiFlagArr[i])
                    break;
            }
            vmi_init(&vmiArr[i], atoi(vmiFunArg[1]), vmiFunArg[2]);
            vmiFlagArr[i]=1;
            sprintf(returnBuf,"%d\0",i);
            returnVal = returnBuf;
        }
        break;

        case 40:
        {
            uint32_t tmp;
            vmi_read_32_va(vmiArr[atoi(vmiFunArg[1])], atol(vmiFunArg[2]), atoi(vmiFunArg[3]), &tmp);
            sprintf(returnBuf, "%u\0", tmp);
            returnVal = returnBuf;
        }
        break;

        case 50:
        {
            addr_t tmp;
            vmi_read_addr_va(vmiArr[atoi(vmiFunArg[1])], atol(vmiFunArg[2]), atoi(vmiFunArg[3]), &tmp);
            sprintf(returnBuf, "%lu\0", tmp);
            returnVal = returnBuf;
        }
        break;
        case 55:
        {
            sprintf(returnBuf, "%s\0", vmi_read_str_va(vmiArr[atoi(vmiFunArg[1])], atol(vmiFunArg[2]), atoi(vmiFunArg[3]) ));
            returnVal = returnBuf;
        }
        break;
        case 67:
        {
            sprintf(returnBuf, "%lu\0", vmi_translate_ksym2v(vmiArr[atoi(vmiFunArg[1])], vmiFunArg[2]) );
            returnVal = returnBuf;
        }
        break;
        default:break;
    }

    write (fd, returnVal, 100);


    return 1;

}

















