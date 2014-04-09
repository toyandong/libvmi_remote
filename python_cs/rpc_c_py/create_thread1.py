import time
import thread
def mytimer(no, interval):
    print "timer"
    cnt = 0
    while cnt<10:
        print 'Thread:(%d) Time:%s/n'%(no, time.ctime())
        time.sleep(interval)
        cnt+=1
    thread.exit_thread()
   
 
def test(): #Use thread.start_new_thread() to create 2 new threads
    print "test"
    thread.start_new_thread(mytimer, (1,1))    
    print "test2"
    thread.start_new_thread(mytimer, (2,2))
 
if __name__=='__main__':
    print "main"
    test()
