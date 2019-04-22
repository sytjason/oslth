#include <linux/kernel.h>
#include <linux/ktime.h>
#include <linux/timekeeping.h>
#include <linux/linkage.h>
#include <linux/uaccess.h>

asmlinkage int sys_mytime(unsigned long* sec, unsigned long* nsec)
{
    struct timespec t;
    getnstimeofday(&t);

    unsigned long res1 = copy_to_user(sec, &(t.tv_sec), 8);
    unsigned long res2 = copy_to_user(nsec, &(t.tv_nsec), 8);

    return 0;
}
