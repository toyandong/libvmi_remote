#---------------------------------------------------------------
#
#
#
#
#
#
#---------------------------------------------------------------

import socket
import time

def pyvmi_rpc_conn(dom0List):
    global pyvmi_rpc_list
    dom0Num = len(dom0List)
    for i in range(dom0Num):
        #print dom0List[i][0],dom0List[i][1]
        pyvmi_rpc_temp=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        pyvmi_rpc_temp.connect((dom0List[i][0],dom0List[i][1]))
        pyvmi_rpc_list += [pyvmi_rpc_temp]
    time.sleep(2)
    
    
def pyvmi_rpc_close(pyvmi_rpc_list):
    dom0Num = len(pyvmi_rpc_list)
    for i in range(dom0Num):
        pyvmi_rpc_list[i].close()

def argType(arg):
    #print type(arg)
    if(type(arg) is int):
        return 'i'
    if(type(arg) is long):
        return 'l'
    if(type(arg) is str):
        return 's'  
    return 's'
#send format is:
#vmid funName funArgType funArg
#funArgType: string(s) int(i) long(l)  
def pyvmi_rpc_send(vmID,funName,*funArg):
    #print "++++++++++++++++++++++pyvmi_rpc_send"
    #print funName, funArg
    funArgType=""
    funArgString=""
    for i in range(len(funArg)):
        funArgType+=argType(funArg[i])
        funArgString+=str(funArg[i])
        if(i==len(funArg)-1):
            break
        funArgString+=" "
    #print str(vmID)+" "+funName+" "+funArgType+" "+funArgString    
    pyvmi_rpc_list[0].send(str(vmID)+" "+funName+" "+funArgType+" "+funArgString)
    returnVal=pyvmi_rpc_list[0].recv(1024)
    #print "++++++++++++++++++++return ", returnVal
    return returnVal
    
def pyvmi_rpc_init(vmList):
    #print "init"
    funArg=""
    for i in range(len(vmList[0])):
        funArg+=vmList[0][i]
        if(i!=len(vmList[0])-1):
            funArg+=" "
    pyvmi_rpc_send(str(-1),"pyvmi.init", funArg)
    
    
    
    
    
def pyvmi_rpc_destroy():
    print "destroy"
    
    
#---------------------------------------------------------------
#Example Code Flow is :
#  socket connect
#  libvmi init
#  senmatic analysis and ...
#  libvmi destroy
#  socket close
#---------------------------------------------------------------

def list_processes(vmID):
    tasksOffset = int(pyvmi_rpc_send(0, "get_offset","linux_tasks"))
    nameOffset = int(pyvmi_rpc_send(0, "get_offset","linux_name"))
    pidOffset = int(pyvmi_rpc_send(0, "get_offset","linux_pid"))
    init_task_va = long(pyvmi_rpc_send(0, "translate_ksym2v","init_task"))
    processes = long(pyvmi_rpc_send(0, "read_addr_va",init_task_va+tasksOffset,0))
    current_process = processes
    while True:
        pid = int(pyvmi_rpc_send(0,"read_32_va",current_process+pidOffset-tasksOffset,0))
        procname = pyvmi_rpc_send(0,"read_str_va",current_process+nameOffset-tasksOffset,0)
        print pid, procname
        current_process = long(pyvmi_rpc_send(0,"read_addr_va",current_process,0))
        if current_process == processes:
            break
    return



#dom0 to be watched
dom0List=[["127.0.0.1",8001],["127.0.0.1",8001]]
#vm to be watched
vmList=[["VM2-ovs","VM2-ovs"],["VM2-ovs","VM2-ovs"]]
#socket handles to stored
pyvmi_rpc_list=[]
#libvmi handles  are stored in dom0. 
#print dom0List[0][0],dom0List[0][1]
pyvmi_rpc_conn(dom0List)
pyvmi_rpc_init(vmList)
list_processes(0)
pyvmi_rpc_close(pyvmi_rpc_list)
	



