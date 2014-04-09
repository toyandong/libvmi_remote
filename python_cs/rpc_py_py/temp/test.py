#通过与函数同名字符串调用函数

def string2fun(arg):
    print "string2fun ", arg
    return "string2fun is printed"

foo=eval('string2fun')
foo("aa")

foo=eval('string2fun')
foo("aa")

class testClass:
    def m1(self):
        print "m1"
    def m2(self):
        print "m2"


T=testClass()
T.m1()
foo=eval("T.m2")
foo()

T1=testClass()
T2=testClass()
classList=[T,T1,T2]
classList[1].m1()

foo=eval("classList[1].m2")
foo()



vmString=" vm1 vm2 vm2 "
print vmString.split(' ')

data="-1 vmi.init vm1 vm2 vm3"
vmID=data.split(' ')[0]
funName=data.split(' ')[1]
funArg=data[len(vmID)+len(funName)+2:]
print vmID,funName,funArg,funArg.split(' ')


def vmifun(funArg1, funArg2):
    print funArg1,funArg2
    
def argType(arg):
    print type(arg)
    if(type(arg) is int):
        return 'i'
    if(type(arg) is long):
        return 'l'
    if(type(arg) is str):
        return 's'  
    return 's'
     
def pyvmi_rpc_send(vmID,funName,*funArg):
    print "pyvmi_rpc_send"
    print funName, funArg
    funArgType=""
    funArgString=""
    for i in range(len(funArg)):
        funArgType+=argType(funArg[i])
        funArgString+=str(funArg[i])
        if(i==len(funArg)-1):
            break
        funArgString+=" "
    funArgString+="!"
    print str(vmID)+" "+funName+" "+funArgType+" "+funArgString
    returnVal="returnval valType"
    return returnVal

pyvmi_rpc_send(-1, "vmifun", "arg1", 1)
funArgType='ssi'
funArgString="linux_tasks linux_tasks 1234"
funArg=funArgString.split(' ')
print funArg,len(funArg)
typedFunArg=[]
for i in range(len(funArg)):
    global typedFunArg
    if(funArgType[i] == 'i'):
        typedFunArg+=[int(funArg[i])]
    elif(funArgType[i] == 'l'):
        typedFunArg+=[long(funArg[i])]
    elif(funArgType[i] == 's'):
        typedFunArg+=[funArg[i]]
    else:
        typedFunArg+=[funArg[i]]
print typedFunArg