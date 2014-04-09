import pyvmi
import sys
vmi = pyvmi.init("VM1-ovs", "complete")
#vmi = pyvmi.init(sys.argv[0], "complete")

def list_processes(vmi):
    tasksOffset = vmi.get_offset("linux_tasks")
    nameOffset = vmi.get_offset("linux_name")
    pidOffset = vmi.get_offset("linux_pid")
    init_task_va = vmi.translate_ksym2v("init_task")
    processes = vmi.read_addr_va(init_task_va+tasksOffset, 0)
    print init_task_va,processes
    print type(tasksOffset),type(nameOffset),type(pidOffset),type(init_task_va),type(processes)
    current_process = processes

    while True:
        pid = vmi.read_32_va(current_process+pidOffset-tasksOffset, 0)
        procname = vmi.read_str_va(current_process+nameOffset-tasksOffset, 0)
        print  current_process+pidOffset-tasksOffset,pid,type(pid)
        print  current_process+nameOffset-tasksOffset,procname,type(procname)

        current_process = vmi.read_addr_va(current_process,0)
        print current_process
        
        if current_process == processes:
            break

    return

list_processes(vmi)

