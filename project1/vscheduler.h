#ifndef _VSCHEDULER_H
#define _VSCHEDULER_H

// Run one unit time //
#define RUN_UINT_TIME() { volatile unsigned long i; for(i=0;i<1000000UL;i++); }
#define POLICY_FIFO     1
#define POLICY_RR       2
#define POLICY_SJF      3
#define POLICY_PSJF     4

struct EasyPCB
{
    char            szName[ 32 ];
    int             nReadyTime;
    int             nExecTime;
    pid_t           nPid;
    unsigned long   nStart_Sec;
    unsigned long   nStart_nSec;
    unsigned long   nEnd_Sec;
    unsigned long   nEnd_nSec;
};

int  AssignProcessToCPU(int nPid, int nCore);
int  LaunchProcess(struct EasyPCB* pPcb);
int  WakeupProcess(int nPid);
void BlockProcess(int nPid);
int  SelectNextProcess(struct EasyPCB *pPcb, int nTotalPcb, int nPolicy);
void DoScheduling(struct EasyPCB *pPcb, int nTotalPcb, int nPolicy);

#endif