import socket
import pyvmi
import sys

#vmiList(dom0) <---> vmList(vmi server)
VMIList={}
def vmiInit(VMName):
    print "init ", VMName
    global VMIList
    VMIList[VMName]=pyvmi.init(VMName, "complete")
    print VMIList
        
def vmiDestroy(VMName):
    print "vmi destroy"
    VMIList.pop(VMName)
    print VMIList

def exeFun(funName,funArgType,funArgString):
    funArg=funArgString.split(' ')
    print "exe", funName,funArgType,funArg
    typedFunArg=[]
    for i in range(len(funArg)):
        if(funArgType[i] == 'i'):
            typedFunArg+=[int(funArg[i])]
        elif(funArgType[i] == 'l'):
            typedFunArg+=[long(funArg[i])]
        elif(funArgType[i] == 's'):
            typedFunArg+=[funArg[i]]
        else:
            typedFunArg+=[funArg[i]]
    print typedFunArg
    foo= eval(funName)
    if(len(funArg) ==1):
        return foo(typedFunArg[0])    
    if(len(funArg) ==2):
        return foo(typedFunArg[0],typedFunArg[1])
    if(len(funArg) ==3):
        return foo(typedFunArg[0],typedFunArg[1],typedFunArg[2])    
    if(len(funArg) ==4):
        return foo(typedFunArg[0],typedFunArg[1],typedFunArg[2],typedFunArg[3])
    print "should not come to here, funArg >=5"
    
    
def handleData(data):
    print "handle data"
    VMName=data.split(' ')[0]
    funName=data.split(' ')[1]
    funArgType=data.split(' ')[2]
    funArg=data[len(VMName)+len(funName)+len(funArgType)+3:] # 3 space
    print VMName, funName, funArgType,funArg
    if(funName == "pyvmi.init"):
        return vmiInit(funArg)
    elif (funName == "pyvmi.destroy"):
        #print "destroy not handled for now"
        return vmiDestroy(funArg)
    else:
        return exeFun("VMIList[\'"+VMName+"\']."+funName, funArgType,funArg)

#main
svr=socket.socket(socket.AF_INET,socket.SOCK_STREAM)  
svr.bind(('127.0.0.1',8004))
svr.listen(5)

while True:
    conn,addr=svr.accept()
    print addr
    print "accept"
    while True:
        data=conn.recv(1024)
        if not data:
            break
        if(data=="exit "):
            print "exit"
            break
        print(data)
        returnVal=handleData(data)
        #type changed
        #returnVal="hello client"
        print type(returnVal),returnVal,len(str(returnVal))
        conn.sendto(str(returnVal),addr)
    conn.close()
