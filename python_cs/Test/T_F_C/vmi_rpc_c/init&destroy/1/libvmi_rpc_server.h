#ifndef LIBVMI_RPC_SERVER_H
#define LIBVMI_RPC_SERVER_H

#include <libvmi/libvmi.h>
#define MAX_DOMU_PER_MACHINE 200


extern void (*vmiFunArr[])();

extern vmi_instance_t vmiArr[MAX_DOMU_PER_MACHINE];
extern int vmiFlagArr[MAX_DOMU_PER_MACHINE];

int rvmi_handle_data(int cfd, char *buf);











#endif
