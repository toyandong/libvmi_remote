#ifndef  INCLUDE_DATA_H
#define  INCLUDE_DATA_H

#include <time.h>
#include "rpc_vmi.h"

/*task_struct中各元素的偏移*/
struct my_task_struct_offset_t
{
	unsigned long comm;
	unsigned long tasks;
	unsigned long pid;
	unsigned long state;
	unsigned long stack;
	unsigned long flags;
	unsigned long policy;
	unsigned long start_time;
	unsigned long real_start_time;
	unsigned long thread;

	unsigned long real_parent;
	unsigned long parent;
	unsigned long children;

	unsigned long fs;
	unsigned long fs_root;
	unsigned long fs_pwd;

	unsigned long files;
	unsigned long files_next_fd;
	unsigned long files_fd_array;
	unsigned long files_fdt;

	unsigned long fdt_max_fds;
	unsigned long fdt_fd;

    unsigned long file_f_path;
	unsigned long file_f_mode;

	unsigned long dentry_d_iname;

	unsigned long init_task;
    unsigned long xtime;
	unsigned long jiffies;

	unsigned long mm;

	int HZ;/*该值存储的不是偏移，是os的频率*/
};


#define TASK_COMM_LEN 16

#define BITS_PER_LONG (sizeof(long)*8)

#define NR_OPEN_DEFAULT BITS_PER_LONG

#define offset_FILE "linux_kernel_offset";


/*节拍jifiies的初试值，该初始值会在5分钟后溢出*/
#define INITIAL_JIFFIES ((unsigned long)(unsigned int) (-300*HZ))
/*节拍的最大值*/
#define MAX_JIFFIES  0xffffffff
/*用来比较节拍，判断溢出*/
#define time_after(a,b)      \
          ((long)(a) - (long)(b) > 0)

/*4.1以上版本支持，以下版本使用 
 *  #define OPEN_XS_DAEMON()     xs_daemon_open()
 *  #define CLOSE_XS_DAEMON(h)   xs_daemon_close(h)
 * */
#define OPEN_XS_DAEMON()    xs_open(0)
#define CLOSE_XS_DAEMON(h)  xs_close(h)

#define XEN_DOM_NAME_LEN  20


/*内核各种信息的偏移*/
extern int parse_offset(struct my_task_struct_offset_t * ts_offset);



#endif
