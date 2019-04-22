#include <linux/linkage.h>
#include <linux/kernel.h>

asmlinkage int sys_myprint(unsigned long start_sec,unsigned long start_nsec,unsigned long end_sec,unsigned long end_nsec,int pid)
{
    printk("[project1] %d %lu.%09lu %lu.%09lu\n",pid, start_sec, start_nsec, end_sec,end_nsec);
    
    return 0;
}