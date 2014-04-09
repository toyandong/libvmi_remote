 /** 该程序使用libvmi来获得VM内存，然后对内存进行语义解析
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
 *用于mysql数据库连接

MYSQL mysql;
char *hostname="localhost";
char *username="root";
char *password="iiecas";
char *dbname="vm_monitor";
char query[300];

*/

/**
 *解析文件的打开模式
 */
static int fmode_parse(unsigned short int fmode, char * str);

/*插入数据库*/
static int insert_vm_process(vmi_instance_t vmi, addr_t vaddr, int pid, char * proname, struct my_task_struct_offset_t *my_ts_off, int dom_id, char *dom_name);
static int insert_vm_file(vmi_instance_t vmi, addr_t vaddr, int pid, struct my_task_struct_offset_t *my_ts_off,int dom_id, char *dom_name);
static int insert_process_create(vmi_instance_t vmi, addr_t vaddr, int pid, struct my_task_struct_offset_t *my_ts_off, int dom_id, char *dom_name);

/**
 *插入进程父子关系
 *vmi--libvmi使用接口
 *vaddr--进程task_struct结构体的起始地址
 *ts_off--存放着kernel中元素的相对偏移地址
 */
static int insert_process_create(vmi_instance_t vmi, addr_t vaddr, int pid, struct my_task_struct_offset_t *ts_off, int dom_id, char *dom_name)
{
    addr_t next_child=0, list_head=0, tmp_child=0, task_ptr;
    int child_pid=0;
	int result;
    unsigned char * procname=NULL;/*进程名字*/

    /*时间*/
    time_t now_time;    /*当前时间*/
    time_t boot_time;   /*进程启动时间*/
    struct tm * ltime;
    char timestr[30], boot_time_str[30];    /*时间的字符串形式*/
    long sec;

    addr_t real_start_time_ptr, xtime_ptr, jiffies_ptr;
    long xtime_sec; /*墙上时间*/
    unsigned long  jiffies; /*系统节拍*/
    unsigned long jiffies_test;
    int HZ =  ts_off->HZ;   /*系统频率，jiffies/HZ=秒数*/

    /*获取子进程*/
    vmi_read_addr_va(vmi, vaddr+ts_off->children, 0, &next_child);/*获得next_child的值*/

    list_head = next_child;/*准备遍历这个双链表*/
    while(1)
    {
        long state; /*进程状态*/
        vmi_read_addr_va(vmi, next_child, 0, &tmp_child);/*存储下一个子进程中list的地址*/
        /*TODO 不是8的原因。 如果减去8，依然有意义，而且获得更多的进程内存不重叠么？*/
        task_ptr = next_child - ts_off->children-8;

        vmi_read_32_va(vmi, task_ptr  + ts_off->pid, 0, &child_pid);//获取子进程的pid
        vmi_read_32_va(vmi, task_ptr +0 , 0, &state);//

        if(child_pid <=0)
		{
			break;
		}

        /*获得进程创建时间*/
        real_start_time_ptr = task_ptr  + ts_off->real_start_time;
        result = vmi_read_32_va(vmi, real_start_time_ptr+0, 0, &sec);//进程结构体中的时间，为距离系统启动的时间

		if( VMI_FAILURE == result)
		{
			//next_child = tmp_child;
			//continue;
			vmi_read_32_va(vmi, real_start_time_ptr+0, 0, &sec);//进程结构体中的时间，为距离系统启动的时间
		}

        //xtime_ptr = (addr_t) 0xc0803980;
        //jiffies_ptr = (addr_t) 0xc06e3b00;
        xtime_ptr = (addr_t) ts_off->xtime; //vm中的cmos时间，即墙上时间
        jiffies_ptr = (addr_t) ts_off->jiffies; // vm中的节拍数

        vmi_read_32_va(vmi, xtime_ptr, 0, &xtime_sec);
        vmi_read_64_va(vmi, jiffies_ptr, 0, &jiffies);

        /*jiffies被初始化为在五分钟后会溢出*/
        jiffies_test = jiffies + HZ;    //给jiffies加上一秒(HZ)
        if( time_after(jiffies, jiffies_test))
        {
            jiffies = MAX_JIFFIES - jiffies;    //如没溢出
        }else
        {
            jiffies = jiffies - INITIAL_JIFFIES;    //如果溢出
        }

        /*墙上时间 减去  节拍数 加上 节拍初始值，得到 系统启动时间，
         *再加上task_struct中记录的具体系统启动的时间sec
         */
        boot_time = (time_t)(xtime_sec - (jiffies/HZ)  + sec );
		if(sec <0)
		{
			boot_time -= sec;
		}
        ltime = localtime(&boot_time);
        memset(boot_time_str,0,sizeof(boot_time_str));
        strftime(boot_time_str,sizeof(boot_time_str),"%Y-%m-%d %H:%M:%S",ltime);
        /*获得当前插入数据的时间*/
        now_time = time(NULL);
        ltime = localtime(&now_time);
        memset(timestr,0,sizeof(timestr));
        strftime(timestr,sizeof(timestr),"%Y-%m-%d %H:%M:%S",ltime);

		//printf("dom_id %d, dom_name %s, parent_id %d, child_id %d, time %s, record_time %s \n", dom_id, dom_name, pid, child_pid, boot_time_str, timestr);
       /*
	   	sprintf(query," insert into process_create(dom_id, dom_name, parent_id, child_id, time, record_time) values(%d,'%s', %d, %d, '%s','%s');",dom_id,dom_name,pid, child_pid, boot_time_str, timestr);
        if(mysql_query(&mysql,query)!=0) //执行插入语句
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
 *插入打开文件信息
 *vmi--libvmi使用接口
 *vaddr--进程task_struct结构体的起始地址
 *ts_off--存放着kernel中元素的相对偏移地址
 */
static int insert_vm_file(vmi_instance_t vmi, addr_t vaddr, int pid, struct my_task_struct_offset_t *my_ts_off,int dom_id, char *dom_name)
{
    int count=0;
    /*存储元素的地址*/
    addr_t files_addr, fdt_ptr, fd_ptr,file_ptr,path_ptr, dentry_ptr, d_iname_ptr, f_mode_ptr;
    addr_t  inode_ptr,ctime_ptr, mtime_ptr, atime_ptr;
    long sec, nsec;

    unsigned short f_mode=0;
    unsigned int max_fds;
    int i;
    unsigned char *d_iname=NULL;

    /*时间*/
    time_t now_time, i_atime, i_mtime, i_ctime, opentime;
    struct tm * ltime;
    char timestr[30], opentime_str[30];
    /*根据结构体跳转*/
    vmi_read_addr_va(vmi, vaddr+ my_ts_off->files, 0, &files_addr );
    vmi_read_addr_va(vmi, files_addr+ my_ts_off->files_fdt, 0, &fdt_ptr );
    vmi_read_addr_va(vmi, fdt_ptr+ my_ts_off->fdt_fd, 0, &fd_ptr );
    vmi_read_addr_va(vmi, fdt_ptr, 0, &max_fds );
    /*遍历打开的所有文件*/
    for(i=0; i<max_fds;i++)
    {
        f_mode = 0;
        /*struct file*/
        vmi_read_addr_va(vmi, fd_ptr+4*i, 0, &file_ptr );
        if(0==file_ptr)
            continue;

        f_mode_ptr=file_ptr + my_ts_off->file_f_mode;
        vmi_read_16_va (vmi, f_mode_ptr, 0, &f_mode);

        /*struct path*/
        path_ptr = file_ptr + my_ts_off->file_f_path;

        /*struct dentry*/
        vmi_read_addr_va(vmi, path_ptr+4, 0, &dentry_ptr );

        /* d_iname*/
        d_iname_ptr = dentry_ptr+ my_ts_off->dentry_d_iname;
        d_iname = vmi_read_str_va(vmi, d_iname_ptr, 0);

        if( NULL==d_iname || !strcmp(d_iname, "") || !strcmp(d_iname, " ") || !strcmp(d_iname, "null"))
        {
            if(d_iname)
            {
                free(d_iname);
                d_iname=NULL;
            }
            continue;
        }

        vmi_read_addr_va(vmi, dentry_ptr+12, 0, &inode_ptr);
        atime_ptr = inode_ptr+76;
        mtime_ptr = inode_ptr+84;
        ctime_ptr = inode_ptr+92;

        if( f_mode & 1)
        {
            vmi_read_32_va(vmi, atime_ptr+0, 0, &sec);
        }else if ( f_mode & 2 )
        {
            vmi_read_32_va(vmi, mtime_ptr+0, 0, &sec);
        }
/*
        printf("d_iname = %s --- ", d_iname);
        printf("f_mode= ");
        fmode_parse(f_mode, NULL);
        printf("\n");
*/

        /*获得当前时间*/
        now_time = time(NULL);
        ltime = localtime(&now_time);
        memset(timestr,0,sizeof(timestr));
        strftime(timestr,sizeof(timestr),"%Y-%m-%d %H:%M:%S",ltime);

        opentime = (time_t) sec;
        ltime = localtime(&opentime);
        memset(opentime_str,0,sizeof(opentime_str));
        strftime(opentime_str,sizeof(opentime_str),"%Y-%m-%d %H:%M:%S",ltime);

		//printf("dom_id %d, dom_name %s, process_id %d,file_name %s, file_mode %d, time %s, record_time %s \n", dom_id, dom_name, pid, d_iname,f_mode, opentime_str, timestr);
        /*
		sprintf(query," insert into vm_file(dom_id, dom_name, process_id, file_name, file_mode, time, record_time) values(%d,'%s', %d, '%s', %d,'%s','%s');",dom_id,dom_name,pid, d_iname, f_mode,opentime_str,timestr);
        if(mysql_query(&mysql,query)!=0) //执行插入语句
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
 *插入进程基本信息
 *vmi--libvmi使用接口
 *vaddr--进程task_struct结构体的起始地址
 *ts_off--存放着kernel中元素的相对偏移地址
 */
static int insert_vm_process(vmi_instance_t vmi, addr_t vaddr, int pid, char * proname, struct my_task_struct_offset_t *my_ts_off, int dom_id, char *dom_name)
{
    addr_t  real_start_time_ptr, sec_ptr, nsec_ptr;
    long sec, nsec, bootsec;

    addr_t xtime_ptr, jiffies_ptr;
    long xtime_sec;
    unsigned long  jiffies;
    unsigned long jiffies_test;
    int HZ =  my_ts_off->HZ;

    /*时间*/
    time_t now_time, boot_time;
    struct tm * ltime;
    char timestr[30], boot_time_str[30];

    /*vm boot time*/
    //addr_t xtime_addr = vmi_translate_ksym2v(vmi, "xtime");
    //printf("xtime_addr is %I64x\n", xtime_addr);
    //vmi_read_32_va(vmi, xtime_addr+0, pid, &bootsec);


    /*real start time*/
    real_start_time_ptr = vaddr+ my_ts_off->real_start_time;
    vmi_read_32_va(vmi, real_start_time_ptr+0, 0, &sec);
    //vmi_read_32_va(vmi, real_start_time_ptr+4, pid, &nsec);

    /*获得进程创建时间*/
    //xtime_ptr = (addr_t) 0xc0803980;
    //jiffies_ptr = (addr_t) 0xc06e3b00;
    xtime_ptr = (addr_t) my_ts_off->xtime;
    jiffies_ptr = (addr_t) my_ts_off->jiffies;


    vmi_read_32_va(vmi, xtime_ptr, 0, &xtime_sec);
    vmi_read_64_va(vmi, jiffies_ptr, 0, &jiffies);

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

    /*获得当前时间*/
    now_time = time(NULL);
    ltime = localtime(&now_time);
    memset(timestr,0,sizeof(timestr));
    strftime(timestr,sizeof(timestr),"%Y-%m-%d %H:%M:%S",ltime);

	//printf("dom_id %d, dom_name %s, process_id %d, process_name %s, time %s, record_time %s \n", dom_id, dom_name, pid, proname, boot_time_str, timestr);
	/*
    sprintf(query," insert into vm_process(dom_id, dom_name, process_id, process_name, time, record_time) values(%d,'%s', %d, '%s', '%s','%s');",dom_id,dom_name,pid, proname,boot_time_str,timestr);
    if(mysql_query(&mysql,query)!=0) //执行插入语句
    {
        printf("insert data error\n");
    }else
    {
        printf("insert data success\n");
    }
	*/
    return 1;
}

/*解析文件的打开模式*/
static int fmode_parse(unsigned short int f_mode, char * str)
{
    if(f_mode & (unsigned short int)1)
        printf("FMODE_READ ");
    if(f_mode & (unsigned short int)2)
        printf("FMODE_WRITE ");
    if(f_mode & (unsigned short int)4)
        printf("FMODE_LSEEK ");
    if(f_mode & (unsigned short int)8)
        printf("FMODE_PREAD  ");
    if(f_mode & (unsigned short int)16)
        printf("FMODE_PWRITE ");
    if(f_mode & (unsigned short int)32)
        printf("FMODE_EXEC ");
    if(f_mode & (unsigned short int)64)
        printf("FMODE_NDELAY ");
    if(f_mode & (unsigned short int)128)
        printf("FMODE_EXCL ");
    if(f_mode & (unsigned short int)256 )
        printf("FMODE_WRITE_IOCTL  ");
    if(f_mode & (unsigned short int)2048)
        printf("FMODE_NOCMTIME ");
    return 1;
}





int mitctl_vmi_main (char *name)
{
    vmi_instance_t vmi;
    unsigned char *memory = NULL;
    uint32_t offset;
    addr_t next_process, list_head;

    /*进程名和进程id*/
    char *procname = NULL;
    int pid = 0;

    int tasks_offset, pid_offset, name_offset;//几个常用的偏移
    status_t status;
	struct my_task_struct_offset_t my_ts_off;//kernel中我所关注元素的偏移，屏蔽不同kernel的差异

	unsigned long  dom_id;
	char *dom_name;

	int count=0;
	int i;

    parse_offset(&my_ts_off);

    vmi_init(&vmi, VMI_AUTO | VMI_INIT_COMPLETE, name);

    tasks_offset = my_ts_off.tasks;
    name_offset = my_ts_off.comm;
    pid_offset = my_ts_off.pid;

    for(i=0;i<100;i++)
    {

    }
    vmi_destroy(vmi);

    return 0;
}





int main()
{
  mitctl_vmi_main("VM2-ovs");
  return 1;
}



