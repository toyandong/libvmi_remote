 /** žÃ³ÌÐòÊ¹ÓÃlibvmiÀŽ»ñµÃVMÄÚŽæ£¬È»ºó¶ÔÄÚŽæœøÐÐÓïÒåœâÎö
  * Author: hanyandong (toyandong@gmail.com)
  * Date: 2013.4
  */

#include <libvmi/libvmi.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <stdio.h>
#include <mysql/mysql.h>
#include <time.h>
#include <string.h>
#include "mitctl_vmi.h"

/**
 *ÓÃÓÚmysqlÊýŸÝ¿âÁ¬œÓ
 
MYSQL mysql;
char *hostname="localhost";
char *username="root";
char *password="iiecas";
char *dbname="vm_monitor";
char query[300];

*/

/**
 *œâÎöÎÄŒþµÄŽò¿ªÄ£Êœ
 */
static int fmode_parse(unsigned  int fmode, char * str);

/*²åÈëÊýŸÝ¿â*/
static int insert_vm_process(int cfd, struct sockaddr * s_addr,struct VMNode * vm, addr_t vaddr, int pid, char * proname, struct my_task_struct_offset_t *my_ts_off,int dom_id);
static int insert_vm_file(int cfd, struct sockaddr * s_addr,struct VMNode * vm, addr_t vaddr, int pid, struct my_task_struct_offset_t *my_ts_off,int dom_id);

static int insert_process_create(int cfd, struct sockaddr * s_addr,struct VMNode * vm, addr_t vaddr, int pid, struct my_task_struct_offset_t *my_ts_off, int dom_id);

/**
 *²åÈëœø³Ìžž×Ó¹ØÏµ
 *vmi--libvmiÊ¹ÓÃœÓ¿Ú
 *vaddr--œø³Ìtask_structœá¹¹ÌåµÄÆðÊŒµØÖ·
 *ts_off--Žæ·Å×ÅkernelÖÐÔªËØµÄÏà¶ÔÆ«ÒÆµØÖ·
 */
static int insert_process_create(int cfd, struct sockaddr * s_addr,struct VMNode * vm, addr_t vaddr, int pid, struct my_task_struct_offset_t *ts_off, int dom_id)
{
    printf("----print_childpid %ld:\n", vaddr);
    addr_t next_child=0, list_head=0, tmp_child=0, task_ptr;
    int child_pid=0;
	int result;
    unsigned char * procname=NULL;/*œø³ÌÃû×Ö*/

    /*Ê±Œä*/
    time_t now_time;    /*µ±Ç°Ê±Œä*/
    time_t boot_time;   /*œø³ÌÆô¶¯Ê±Œä*/
    struct tm * ltime;
    char timestr[30], boot_time_str[30];    /*Ê±ŒäµÄ×Ö·ûŽ®ÐÎÊœ*/
    long sec;

    addr_t real_start_time_ptr, xtime_ptr, jiffies_ptr;
    long xtime_sec; /*ÇœÉÏÊ±Œä*/
    unsigned long  jiffies; /*ÏµÍ³œÚÅÄ*/
    unsigned long jiffies_test;
    int HZ =  ts_off->HZ;   /*ÏµÍ³ÆµÂÊ£¬jiffies/HZ=ÃëÊý*/

    /*»ñÈ¡×Óœø³Ì*/
    //vmi_read_addr_va(vmi, vaddr+ts_off->children, 0, &next_child);/*»ñµÃnext_childµÄÖµ*/
    next_child = atol( vmi_rpc_send(cfd, s_addr, vm->name, "read_addr_va", "li", vaddr+ts_off->children, 0) );
    printf("children is %ld \n ", vaddr+ts_off->children);

    list_head = next_child;/*×Œ±ž±éÀúÕâžöË«ÁŽ±í*/
    while(1)
    {
        long state; /*œø³Ì×ŽÌ¬*/
        //vmi_read_addr_va(vmi, next_child, 0, &tmp_child);/*ŽæŽ¢ÏÂÒ»žö×Óœø³ÌÖÐlistµÄµØÖ·*/
        tmp_child = atol( vmi_rpc_send(cfd, s_addr, vm->name, "read_addr_va", "li", next_child, 0) );
        /*TODO ²»ÊÇ8µÄÔ­Òò¡£ Èç¹ûŒõÈ¥8£¬ÒÀÈ»ÓÐÒâÒå£¬¶øÇÒ»ñµÃžü¶àµÄœø³ÌÄÚŽæ²»ÖØµþÃŽ£¿*/
        task_ptr = next_child - ts_off->children-8;
        printf("next_child is %ld \n ", next_child);
        printf("tmp_child addr is %ld \n", tmp_child);

        //vmi_read_32_va(vmi, task_ptr  + ts_off->pid, 0, &child_pid);//»ñÈ¡×Óœø³ÌµÄpid
        //vmi_read_32_va(vmi, task_ptr +0 , 0, &state);//
        child_pid = atoi ( vmi_rpc_send(cfd, s_addr, vm->name, "read_32_va", "li", task_ptr  + ts_off->pid, 0) );
        state = atoi ( vmi_rpc_send(cfd, s_addr, vm->name, "read_32_va", "li", task_ptr +0, 0) );

        printf("--> [%d %ld]\n", child_pid, state);

        if(child_pid <=0)
		{
			break;
		}

        /*»ñµÃœø³ÌŽŽœšÊ±Œä*/
        real_start_time_ptr = task_ptr  + ts_off->real_start_time;
        //result = vmi_read_32_va(vmi, real_start_time_ptr+0, 0, &sec);//œø³Ìœá¹¹ÌåÖÐµÄÊ±Œä£¬ÎªŸàÀëÏµÍ³Æô¶¯µÄÊ±Œä
        sec = atol ( vmi_rpc_send(cfd, s_addr, vm->name, "read_32_va", "li", real_start_time_ptr+0, 0) );

		if( VMI_FAILURE == result)
		{
			printf("cpid =%d result fail\n", child_pid);
			//next_child = tmp_child;
			//continue;
			//vmi_read_32_va(vmi, real_start_time_ptr+0, 0, &sec);//œø³Ìœá¹¹ÌåÖÐµÄÊ±Œä£¬ÎªŸàÀëÏµÍ³Æô¶¯µÄÊ±Œä
		}

        //xtime_ptr = (addr_t) 0xc0803980;
        //jiffies_ptr = (addr_t) 0xc06e3b00;
        xtime_ptr = (addr_t) ts_off->xtime; //vmÖÐµÄcmosÊ±Œä£¬ŒŽÇœÉÏÊ±Œä
        jiffies_ptr = (addr_t) ts_off->jiffies; // vmÖÐµÄœÚÅÄÊý

        //vmi_read_32_va(vmi, xtime_ptr, 0, &xtime_sec);
        //vmi_read_64_va(vmi, jiffies_ptr, 0, &jiffies);
        xtime_sec = atol ( vmi_rpc_send(cfd, s_addr, vm->name, "read_32_va", "li", xtime_ptr, 0) );
        jiffies = atol ( vmi_rpc_send(cfd, s_addr, vm->name, "read_64_va", "li", jiffies_ptr, 0) );

        /*jiffies±»³õÊŒ»¯ÎªÔÚÎå·ÖÖÓºó»áÒç³ö*/
        jiffies_test = jiffies + HZ;    //žøjiffiesŒÓÉÏÒ»Ãë(HZ)
        if( time_after(jiffies, jiffies_test))
        {
            jiffies = MAX_JIFFIES - jiffies;    //ÈçÃ»Òç³ö
        }else
        {
            jiffies = jiffies - INITIAL_JIFFIES;    //Èç¹ûÒç³ö
        }

        /*ÇœÉÏÊ±Œä ŒõÈ¥  œÚÅÄÊý ŒÓÉÏ œÚÅÄ³õÊŒÖµ£¬µÃµœ ÏµÍ³Æô¶¯Ê±Œä£¬
         *ÔÙŒÓÉÏtask_structÖÐŒÇÂŒµÄŸßÌåÏµÍ³Æô¶¯µÄÊ±Œäsec
         */
        boot_time = (time_t)(xtime_sec - (jiffies/HZ)  + sec );
		if(sec <0)
		{
			boot_time -= sec;
		}
        ltime = localtime(&boot_time);
        memset(boot_time_str,0,sizeof(boot_time_str));
        strftime(boot_time_str,sizeof(boot_time_str),"%Y-%m-%d %H:%M:%S",ltime);
        /*»ñµÃµ±Ç°²åÈëÊýŸÝµÄÊ±Œä*/
        now_time = time(NULL);
        ltime = localtime(&now_time);
        memset(timestr,0,sizeof(timestr));
        strftime(timestr,sizeof(timestr),"%Y-%m-%d %H:%M:%S",ltime);
		
		//printf("dom_id %d, dom_name %s, parent_id %d, child_id %d, time %s, record_time %s \n", dom_id, vm->name, pid, child_pid, boot_time_str, timestr);
		printf("childid is %d",child_pid);
       /*
	   	sprintf(query," insert into process_create(dom_id, dom_name, parent_id, child_id, time, record_time) values(%d,'%s', %d, %d, '%s','%s');",dom_id,dom_name,pid, child_pid, boot_time_str, timestr);
        if(mysql_query(&mysql,query)!=0) //ÖŽÐÐ²åÈëÓïŸä
        {
            printf("insert data error\n");
        }else
        {
            printf("insert data success\n");
        }
		*/

        next_child = tmp_child;
        if(list_head == tmp_child)
        {
              break;
        }

    }
    return 1;
}

/**
 *²åÈëŽò¿ªÎÄŒþÐÅÏ¢
 *vmi--libvmiÊ¹ÓÃœÓ¿Ú
 *vaddr--œø³Ìtask_structœá¹¹ÌåµÄÆðÊŒµØÖ·
 *ts_off--Žæ·Å×ÅkernelÖÐÔªËØµÄÏà¶ÔÆ«ÒÆµØÖ·
 */
static int insert_vm_file(int cfd, struct sockaddr * s_addr,struct VMNode * vm, addr_t vaddr, int pid, struct my_task_struct_offset_t *my_ts_off,int dom_id)
{
    int count=0;
    printf("----print_files_info:\n");
    /*ŽæŽ¢ÔªËØµÄµØÖ·*/
    addr_t files_addr, fdt_ptr, fd_ptr,file_ptr,path_ptr, dentry_ptr, d_iname_ptr, f_mode_ptr;
    addr_t  inode_ptr,ctime_ptr, mtime_ptr, atime_ptr;
    long sec, nsec;

    unsigned int f_mode=0;
    unsigned int max_fds;
    int i;
    unsigned char *d_iname=NULL;

    /*Ê±Œä*/
    time_t now_time, i_atime, i_mtime, i_ctime, opentime;
    struct tm * ltime;
    char timestr[30], opentime_str[30];
    /*žùŸÝœá¹¹ÌåÌø×ª*/
    //vmi_read_addr_va(vmi, vaddr+ my_ts_off->files, pid, &files_addr );
    //vmi_read_addr_va(vmi, files_addr+ my_ts_off->files_fdt, pid, &fdt_ptr );
    //vmi_read_addr_va(vmi, fdt_ptr+ my_ts_off->fdt_fd, pid, &fd_ptr );
    //vmi_read_addr_va(vmi, fdt_ptr, pid, &max_fds );
    files_addr = atol( vmi_rpc_send(cfd, s_addr, vm->name, "read_addr_va", "li", vaddr+ my_ts_off->files, 0) );
    fdt_ptr = atol( vmi_rpc_send(cfd, s_addr, vm->name, "read_addr_va", "li", files_addr+ my_ts_off->files_fdt, 0) );
    fd_ptr = atol( vmi_rpc_send(cfd, s_addr, vm->name, "read_addr_va", "li", fdt_ptr+ my_ts_off->fdt_fd, 0) );
    max_fds = atoi( vmi_rpc_send(cfd, s_addr, vm->name, "read_addr_va", "li", fdt_ptr, 0) );
    
    printf("max_fds : %d\n", max_fds);
    /*±éÀúŽò¿ªµÄËùÓÐÎÄŒþ*/
    for(i=0; i<max_fds;i++)
    {
        f_mode = 0;
        /*struct file*/
        //vmi_read_addr_va(vmi, fd_ptr+4*i, pid, &file_ptr );
        file_ptr = atol( vmi_rpc_send(cfd, s_addr, vm->name, "read_addr_va", "li", fd_ptr+4*i, 0) );
        
        if(0==file_ptr)
            continue;

        f_mode_ptr=file_ptr + my_ts_off->file_f_mode;
        //vmi_read_16_va (vmi, f_mode_ptr, pid, &f_mode);
		f_mode = atoi ( vmi_rpc_send(cfd, s_addr, vm->name, "read_16_va", "li", f_mode_ptr, 0) );
        /*struct path*/
        path_ptr = file_ptr + my_ts_off->file_f_path;

        /*struct dentry*/
        //vmi_read_addr_va(vmi, path_ptr+4, pid, &dentry_ptr );
		dentry_ptr = atol( vmi_rpc_send(cfd, s_addr, vm->name, "read_addr_va", "li", path_ptr+4, 0) );

        /* d_iname*/
        d_iname_ptr = dentry_ptr+ my_ts_off->dentry_d_iname;

        //d_iname = vmi_read_str_va(vmi, d_iname_ptr, 0);
        d_iname = vmi_rpc_send(cfd, s_addr, vm->name, "read_str_va", "li", d_iname_ptr, pid);

        if( NULL==d_iname || !strcmp(d_iname, "") || !strcmp(d_iname, "0") || !strcmp(d_iname, " ") || !strcmp(d_iname, "null") || !strcmp(d_iname, "NULL"))
        {
            if(d_iname)
            {
                free(d_iname);
                d_iname=NULL;
            }
            continue;
        }

        //vmi_read_addr_va(vmi, dentry_ptr+12, pid, &inode_ptr);
        inode_ptr = atol( vmi_rpc_send(cfd, s_addr, vm->name, "read_addr_va", "li", dentry_ptr+12, 0) );
        atime_ptr = inode_ptr+76;
        mtime_ptr = inode_ptr+84;
        ctime_ptr = inode_ptr+92;

        if( f_mode & 1)
        {
            //vmi_read_32_va(vmi, atime_ptr+0, pid, &sec);
            sec = atol( vmi_rpc_send(cfd, s_addr, vm->name, "read_addr_va", "li", atime_ptr+0, 0) );
        }else if ( f_mode & 2 )
        {
            //vmi_read_32_va(vmi, mtime_ptr+0, pid, &sec);
            sec = atol( vmi_rpc_send(cfd, s_addr, vm->name, "read_addr_va", "li", mtime_ptr+0, 0) );
            
        }
/*
        printf("d_iname = %s --- ", d_iname);
        printf("f_mode= ");
        fmode_parse(f_mode, NULL);
        printf("\n");
*/

        /*»ñµÃµ±Ç°Ê±Œä*/
        now_time = time(NULL);
        ltime = localtime(&now_time);
        memset(timestr,0,sizeof(timestr));
        strftime(timestr,sizeof(timestr),"%Y-%m-%d %H:%M:%S",ltime);

        opentime = (time_t) sec;
        ltime = localtime(&opentime);
        memset(opentime_str,0,sizeof(opentime_str));
        strftime(opentime_str,sizeof(opentime_str),"%Y-%m-%d %H:%M:%S",ltime);

		//printf("dom_id %d, dom_name %s, process_id %d,file_name %s, file_mode %d, time %s, record_time %s \n", dom_id, vm->name, pid, d_iname,f_mode, opentime_str, timestr);
		printf("%s\n",d_iname);
        /*
		sprintf(query," insert into vm_file(dom_id, dom_name, process_id, file_name, file_mode, time, record_time) values(%d,'%s', %d, '%s', %d,'%s','%s');",dom_id,dom_name,pid, d_iname, f_mode,opentime_str,timestr);
        if(mysql_query(&mysql,query)!=0) //ÖŽÐÐ²åÈëÓïŸä
        {
            printf("insert data error\n");
        }else
        {
            printf("insert data success\n");
        }
		*/

        if(d_iname)
        {
            free(d_iname);
            d_iname=NULL;
        }

    }
    return 1;
}

/**
 *²åÈëœø³Ì»ù±ŸÐÅÏ¢
 *vmi--libvmiÊ¹ÓÃœÓ¿Ú
 *vaddr--œø³Ìtask_structœá¹¹ÌåµÄÆðÊŒµØÖ·
 *ts_off--Žæ·Å×ÅkernelÖÐÔªËØµÄÏà¶ÔÆ«ÒÆµØÖ·
 */
static int insert_vm_process(int cfd, struct sockaddr * s_addr,struct VMNode * vm, addr_t vaddr, int pid, char * proname, struct my_task_struct_offset_t *my_ts_off, int dom_id)
{
	printf("vm_process \n");
    addr_t  real_start_time_ptr, sec_ptr, nsec_ptr;
    long sec, nsec, bootsec;

    addr_t xtime_ptr, jiffies_ptr;
    long xtime_sec;
    unsigned long  jiffies;
    unsigned long jiffies_test;
    int HZ =  my_ts_off->HZ;

    /*Ê±Œä*/
    time_t now_time, boot_time;
    struct tm * ltime;
    char timestr[30], boot_time_str[30];

    /*vm boot time*/
    //addr_t xtime_addr = vmi_translate_ksym2v(vmi, "xtime");
    //printf("xtime_addr is %I64x\n", xtime_addr);
    //vmi_read_32_va(vmi, xtime_addr+0, pid, &bootsec);


    /*real start time*/
    real_start_time_ptr = vaddr+ my_ts_off->real_start_time;
    //vmi_read_32_va(vmi, real_start_time_ptr+0, pid, &sec);
    sec=atol(vmi_rpc_send(cfd,s_addr,vm->name,"read_32_va","li",real_start_time_ptr+0 , 0 ));
    //vmi_read_32_va(vmi, real_start_time_ptr+4, pid, &nsec);

    /*»ñµÃœø³ÌŽŽœšÊ±Œä*/
    //xtime_ptr = (addr_t) 0xc0803980;
    //jiffies_ptr = (addr_t) 0xc06e3b00;
    xtime_ptr = (addr_t) my_ts_off->xtime;
    jiffies_ptr = (addr_t) my_ts_off->jiffies;

    //vmi_read_32_va(vmi, xtime_ptr, 0, &xtime_sec);
    //vmi_read_32_va(vmi, jiffies_ptr, 0, &jiffies);
    xtime_sec = atol(vmi_rpc_send(cfd,s_addr,vm->name,"read_32_va","ii", xtime_ptr , 0));
    jiffies = atol(vmi_rpc_send(cfd,s_addr,vm->name,"read_64_va","ii", jiffies_ptr , 0));
    printf("xtime_ptr is %lu  %ld\n", my_ts_off->xtime, xtime_ptr);
    printf("jiffies is %lu   %ld\n",jiffies,  jiffies_ptr);

    jiffies_test = jiffies + HZ;

    if( time_after(jiffies, jiffies_test) )
    {
        jiffies = MAX_JIFFIES - jiffies;
    }else
    {
        jiffies = jiffies - INITIAL_JIFFIES;
    }

    boot_time = (time_t)(xtime_sec - (jiffies/HZ)  + sec);
    if( sec < 0)
    {
        boot_time -= sec;
    }

    ltime = localtime(&boot_time);
    memset(boot_time_str,0,sizeof(boot_time_str));
    strftime(boot_time_str,sizeof(boot_time_str),"%Y-%m-%d %H:%M:%S",ltime);
    printf("bootsec= %ld, sec= %ld", boot_time, sec);
    printf("creat time is  %s\n", boot_time_str);

    /*»ñµÃµ±Ç°Ê±Œä*/
    now_time = time(NULL);
    ltime = localtime(&now_time);
    memset(timestr,0,sizeof(timestr));
    strftime(timestr,sizeof(timestr),"%Y-%m-%d %H:%M:%S",ltime);

	printf("dom_id %d, dom_name %s, process_id %d, process_name %s, time %s, record_time %s \n", dom_id, vm->name, pid, proname, boot_time_str, timestr);
	/*
    sprintf(query," insert into vm_process(dom_id, dom_name, process_id, process_name, time, record_time) values(%d,'%s', %d, '%s', '%s','%s');",dom_id,dom_name,pid, proname,boot_time_str,timestr);
    if(mysql_query(&mysql,query)!=0) //ÖŽÐÐ²åÈëÓïŸä
    {
        printf("insert data error\n");
    }else
    {
        printf("insert data success\n");
    }
	*/
    return 1;
}

/*œâÎöÎÄŒþµÄŽò¿ªÄ£Êœ*/
static int fmode_parse(unsigned  int f_mode, char * str)
{
    if(f_mode & (unsigned  int)1)
        printf("FMODE_READ ");
    if(f_mode & (unsigned  int)2)
        printf("FMODE_WRITE ");
    if(f_mode & (unsigned  int)4)
        printf("FMODE_LSEEK ");
    if(f_mode & (unsigned  int)8)
        printf("FMODE_PREAD  ");
    if(f_mode & (unsigned  int)16)
        printf("FMODE_PWRITE ");
    if(f_mode & (unsigned  int)32)
        printf("FMODE_EXEC ");
    if(f_mode & (unsigned  int)64)
        printf("FMODE_NDELAY ");
    if(f_mode & (unsigned  int)128)
        printf("FMODE_EXCL ");
    if(f_mode & (unsigned  int)256 )
        printf("FMODE_WRITE_IOCTL  ");
    if(f_mode & (unsigned  int)2048)
        printf("FMODE_NOCMTIME ");
    return 1;
}





int mitctl_vmi_main (void * vm_info)
{
    unsigned char *memory = NULL;
    uint32_t offset;
    addr_t next_process, list_head;

    /*œø³ÌÃûºÍœø³Ìid*/
    char *procname = NULL;
    int pid = 0;

    int tasks_offset, pid_offset, name_offset;//Œžžö³£ÓÃµÄÆ«ÒÆ
    status_t status;
    struct my_task_struct_offset_t my_ts_off;//kernelÖÐÎÒËù¹Ø×¢ÔªËØµÄÆ«ÒÆ£¬ÆÁ±Î²»Í¬kernelµÄ²îÒì
    unsigned long  dom_id;
    //char *dom_name;
    int count=0;
	
    /*socket connect*/
    struct VMNode * vm=(struct VMNode *)vm_info;
    printf("hello fun:%s, %u, %s\n",inet_ntoa(vm->ip),vm->port, vm->name);
    /*建立socket链接*/
    int cfd;
    char buffer[1024]="hello c socket livmi";
    struct sockaddr_in s_add;
    

    cfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&s_add,sizeof(struct sockaddr_in));
    s_add.sin_family=AF_INET;
    s_add.sin_addr.s_addr=vm->ip.s_addr;
    s_add.sin_port=htons(vm->port);
    printf("s_addr = %#x ,port : %#x\r\n",s_add.sin_addr.s_addr,s_add.sin_port);

    connect(cfd,(struct sockaddr *)(&s_add), sizeof(struct sockaddr));
  
    struct sockaddr * s_addr_arg=(struct sockaddr *)(&s_add);
    
    /*socket connect over*/


    /*³õÊŒ»¯ÊýŸÝ¿âÁ¬œÓ*/
/*
    mysql_init(&mysql);
    if(!mysql_real_connect(&mysql,hostname,username,password,dbname,0,NULL,0))
    {
           printf("connect database fail !!!\n");
    }
    else
    {
           printf("connect database successful !!!\n");
    }
*/
    /*»ñÈ¡Æ«ÒÆÖµ*/
    parse_offset(&my_ts_off);
    
    vmi_rpc_init(cfd,(struct sockaddr *)(&s_add),vm->name);

    tasks_offset = my_ts_off.tasks;
    name_offset = my_ts_off.comm;
    pid_offset = my_ts_off.pid;

    /*TODO  pause the vm for consistent memory access
   if (vmi_pause_vm(vmi) != VMI_SUCCESS)
    {
       printf("Failed to pause VM\n");
        goto error_exit;
    }
    */

    //vmi_read_addr_va(vmi, my_ts_off.init_task + tasks_offset, 0, &next_process);
    next_process=atol( vmi_rpc_send(cfd,s_addr_arg,vm->name,"read_addr_va","li",my_ts_off.init_task + tasks_offset,0) );


    /*¿ªÊŒ±éÀúœø³ÌË«ÁŽ±í*/
    list_head = next_process;
    /* walk the task list */
    while (1){
        /* follow the next pointer */
        addr_t tmp_next = 0;
        //vmi_read_addr_va(vmi, next_process, 0, &tmp_next);
       tmp_next=atol( vmi_rpc_send(cfd,s_addr_arg,vm->name, "read_addr_va", "li", next_process, 0) );
		
        /* if we are back at the list head, we are done */
        if (list_head == tmp_next){
            printf("I am breaking list_head %lx, tmp_next %lx\n", list_head, tmp_next);
			break;
        }

        /* print out the process name */
       // procname = vmi_read_str_va(vmi, next_process + name_offset - tasks_offset, 0);
		procname=vmi_rpc_send(cfd, s_addr_arg, vm->name, "read_str_va", "li", next_process + name_offset - tasks_offset, 0);
		
        if (!procname) {
            printf ("Failed to find procname\n");
        } // if

        //vmi_read_32_va(vmi, next_process + pid_offset - tasks_offset, 0, &pid);
        pid=atoi( vmi_rpc_send(cfd, s_addr_arg, vm->name, "read_32_va", "li", next_process + pid_offset - tasks_offset, 0) );

        long state;
        //vmi_read_32_va(vmi, next_process  - tasks_offset, 0, &state);
        state=atoi( vmi_rpc_send(cfd, s_addr_arg, vm->name, "read_32_va", "li", next_process - tasks_offset, 0) );

        /* trivial sanity check on data */
        if (pid >= 0){
            printf("============================================================================\n");
            printf("[%5d] %s %ld\n", pid, procname, state);
            insert_vm_process(cfd, s_addr_arg, vm , next_process-tasks_offset, pid, procname,&my_ts_off, dom_id);
            insert_vm_file(cfd, s_addr_arg, vm, next_process-tasks_offset, pid, &my_ts_off, dom_id);
            insert_process_create(cfd, s_addr_arg, vm, next_process-tasks_offset, pid, &my_ts_off, dom_id);

        }
        if (procname){
            free(procname);
            procname = NULL;
        }
        next_process = tmp_next;
    }

error_exit:
    if(procname) free(procname);

    /*TODO  resume the vm 
    vmi_resume_vm(vmi);
	*/
	
    /* cleanup any memory associated with the LibVMI instance */
    //vmi_destroy(vmi);
	vmi_rpc_destroy(cfd, (struct sockaddr *)(&s_add), vm->name);
    /*断开socket链接*/
    close(cfd);

    /*¹Ø±ÕÊýŸÝ¿âÁ¬œÓ*/
    // mysql_close(&mysql);
    return 0;
}





int main()
{
    struct VMNode VMList[1];

    VMList[0].port=8003;
    VMList[0].ip.s_addr=inet_addr("127.0.0.1");
    memcpy(VMList[0].name, "VM2-ovs", sizeof("VM2-ovs"));
  	mitctl_vmi_main(&VMList[0]);
 	return 1;
}


