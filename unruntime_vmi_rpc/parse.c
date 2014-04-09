#include <stdio.h>
#include "mitctl_vmi.h"

static int my_map(char *key, unsigned long offset, struct my_task_struct_offset_t * ts_offset);
/**/
int parse_offset(struct my_task_struct_offset_t * ts_offset)
{
	FILE *fp;
	char key[50]={'\n'};
	unsigned long offset;
    fp = fopen("linux_kernel_offset", "r+");
	if( fp ==NULL )
	{
		printf("error!");
		return -1;
	}

	while (!feof(fp))
	{
		fscanf(fp, "%s", key);
		fscanf(fp, "%lu", &offset);
		//printf("%s %ld \n", key, offset);
		my_map(key, offset, ts_offset);
		key[0]='\0';
	}
	fclose(fp);

	printf("test...\n");
	printf("%ld, %ld, %ld \n", ts_offset->comm, ts_offset->pid, ts_offset->tasks);

	return 0;
}

static int my_map(char *key, unsigned long offset, struct my_task_struct_offset_t * ts_offset)
{
	if( '\0'==key[0]  || '\n'==key[0] || ' '==key[0] || '#'==key[0]) return 0;

	if( 0 == strcmp(key, "comm_offset"))
	{
		printf("found : %s = %ld\n", key, offset);
		ts_offset->comm = offset;
	}else if( 0 == strcmp(key, "stack_offset"))
	{
		printf("found : %s = %ld\n", key, offset);
		ts_offset->stack = offset;
	}
	else if( 0 == strcmp(key, "tasks_offset"))
	{
		printf("found : %s = %ld\n", key, offset);
		ts_offset->tasks = offset;
	}
	else if( 0 == strcmp(key, "pid_offset"))
	{
		printf("found : %s = %ld\n", key, offset);
		ts_offset->pid = offset;
	}
	else if( 0 == strcmp(key, "state_offset"))
	{
		printf("found : %s = %ld\n", key, offset);
		ts_offset->state = offset;
	}else if( 0 == strcmp(key, "flags_offset"))
	{
		printf("found : %s = %ld\n", key, offset);
		ts_offset->flags = offset;
	}else if( 0 == strcmp(key, "policy_offset"))
	{
		printf("found : %s = %ld\n", key, offset);
		ts_offset->policy = offset;
	}else if( 0 == strcmp(key, "start_time_offset"))
	{
		printf("found : %s = %ld\n", key, offset);
		ts_offset->start_time = offset;
	}else if( 0 == strcmp(key, "real_start_time_offset"))
	{
		printf("found : %s = %ld\n", key, offset);
		ts_offset->real_start_time = offset;
	}else if( 0 == strcmp(key, "thread_offset"))
	{
		printf("found : %s = %ld\n", key, offset);
		ts_offset->thread = offset;
	}else if( 0 == strcmp(key, "parent_offset"))
	{
		printf("found : %s = %ld\n", key, offset);
		ts_offset->parent = offset;
	}else if( 0 == strcmp(key, "real_parent_offset"))
	{
		printf("found : %s = %ld\n", key, offset);
		ts_offset->real_parent = offset;
	}else if( 0 == strcmp(key, "children_offset"))
	{
		printf("found : %s = %ld\n", key, offset);
		ts_offset->children = offset;
	}else if( 0 == strcmp(key, "mm_offset"))/*mm_struct info*/
	{
		printf("found : %s = %ld\n", key, offset);
		ts_offset->mm = offset;
	}else if( 0 == strcmp(key, "fs_offset")) /*fs_struct info*/
	{
		printf("found : %s = %ld\n", key, offset);
		ts_offset->fs = offset;
		printf("ok : %s = %ld\n", key, ts_offset->fs);
	}else if( 0 == strcmp(key, "fs_root_offset"))
	{
		printf("found : %s = %ld\n", key, offset);
		ts_offset->fs_root = offset;
	}else if( 0 == strcmp(key, "fs_pwd_offset"))
	{
		printf("found : %s = %ld\n", key, offset);
		ts_offset->fs_pwd = offset;
	}else if( 0 == strcmp(key, "files_offset")) /*file*/
	{
		printf("found : %s = %ld\n", key, offset);
		ts_offset->files = offset;
	}else if( 0 == strcmp(key, "files_next_fd_offset"))
	{
		printf("found : %s = %ld\n", key, offset);
		ts_offset->files_next_fd = offset;
	}else if( 0 == strcmp(key, "files_fd_array_offset"))
	{
		printf("found : %s = %ld\n", key, offset);
		ts_offset->files_fd_array = offset;
	}else if( 0 == strcmp(key, "dentry_d_iname_offset"))
	{
		printf("found : %s = %ld\n", key, offset);
		ts_offset->dentry_d_iname = offset;
	}else if( 0 == strcmp(key, "file_f_mode_offset"))
	{
		printf("found : %s = %ld\n", key, offset);
		ts_offset->file_f_mode = offset;
	}
	else if( 0 == strcmp(key, "file_f_path_offset"))
	{
		printf("found : %s = %ld\n", key, offset);
		ts_offset->file_f_path = offset;
	}else if( 0 == strcmp(key, "fdt_max_fds_offset"))
	{
		printf("found : %s = %ld\n", key, offset);
		ts_offset->fdt_max_fds = offset;
	}else if( 0 == strcmp(key, "files_fdt_offset"))
	{
		printf("found : %s = %ld\n", key, offset);
		ts_offset->files_fdt = offset;
	}else if( 0 == strcmp(key, "fdt_fd_offset"))
	{
		printf("found : %s = %ld\n", key, offset);
		ts_offset->fdt_fd = offset;
	}else if( 0 == strcmp(key, "HZ_value"))/*Ê±¼ä*/
	{
		printf("found : %s = %ld\n", key, offset);
		ts_offset->HZ = offset;
	}else if( 0 == strcmp(key, "jiffies_addr"))
	{
		printf("found : %s = %lu\n", key, offset);
		ts_offset->jiffies = offset;
	}else if( 0 == strcmp(key, "xtime_addr"))
	{
		printf("found : %s = %lu\n", key, offset);
		ts_offset->xtime = offset;
	}else if( 0 == strcmp(key, "init_task_addr"))
	{
		printf("found : %s = %lu\n", key, offset);
		ts_offset->init_task = offset;
	}else
	{
		printf("not found : %s = %ld\n", key, offset);
		return 0;
	}
	return 1;
}












