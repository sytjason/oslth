#include <stdio.h>
#include <stdlib.h>
#define __USE_GNU
#include <sched.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include "vscheduler.h"
#include "queue.h"

#define SYS_GET_TIME        334
#define SYS_PRINTK          335 
#define GET_START_TIME      1
#define DISPLAY_END_TIME    0
#define CPU_FOR_PARENT      0
#define CPU_FOR_CHILD       1

static int nRunningProc     = -1;
static int nUnitTime        =  0;
static int nLastUnitTime    =  0;
static queue proc_queue;
int AssignProcessToCPU(int nPid, int nCore)
{
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(nCore, &mask);

    return sched_setaffinity(nPid, sizeof(mask), &mask);
}

int LaunchProcess(struct EasyPCB* pPcb)
{
    pid_t nPid = fork();

    if (nPid < 0)
    {
        perror("Fail to fork.");
        return -1;
    }

    if (nPid == 0)
    {
        pid_t nChildPid = getpid();

        if (AssignProcessToCPU(nChildPid, CPU_FOR_CHILD))
        {
            printf("Fail to AssignProcessToCPU [ CPU_FOR_CHILD ].\n");
        }

        syscall(SYS_GET_TIME, &(pPcb->nStart_Sec), &(pPcb->nStart_nSec));
        
        int i;
        int nExeTime = pPcb->nExecTime;
        for (i=0; i<nExeTime; ++i)
        {
            RUN_UINT_TIME();
        }

        printf("##Pid = %d finished.\n", nChildPid);

        syscall(SYS_GET_TIME, &(pPcb->nEnd_Sec), &(pPcb->nEnd_nSec));
        syscall(SYS_PRINTK, pPcb->nStart_Sec, pPcb->nStart_nSec, pPcb->nEnd_Sec, pPcb->nEnd_nSec, nChildPid);

        exit(0);
    }

    return nPid;
}

int WakeupProcess(int nPid)
{
    struct sched_param  param;
    // Note : For SCHED_OTHER policy, sched_priority must be 0. //
    param.sched_priority = 0;
    return sched_setscheduler(nPid, SCHED_OTHER, &param);
}

void BlockProcess(int nPid)
{
    struct sched_param  param;
    // Note : For SCHED_IDLE policy, sched_priority must be 0. //
    param.sched_priority = 0;
    sched_setscheduler(nPid, SCHED_IDLE, &param);
}

int SelectNextProcess(struct EasyPCB *pPcb, int nTotalPcb, int nPolicy)
{
    int i;
    int nNextProc = -1;

    if (nPolicy == POLICY_FIFO)
    {
        for (i=0; i<nTotalPcb; ++i)
        {
            if (pPcb[ i ].nPid == -1 || pPcb[ i ].nExecTime <= 0)
                continue;
            
            if (nNextProc == -1 || pPcb[ i ].nReadyTime < pPcb[ nNextProc ].nReadyTime)
            {
                nNextProc = i;
            }
        }
    }
    else
    if (nPolicy == POLICY_RR)
    {
        if(nRunningProc == -1)
        {
            if(q_isEmpty(&proc_queue)==0)
            {
                nNextProc = q_peek(&proc_queue);
                q_remove(&proc_queue);
            }
        }
        else if((nUnitTime - nLastUnitTime) % 500 == 0)
        {
            if(pPcb[nRunningProc].nExecTime!=0)
                q_insert(&proc_queue,nRunningProc);
            nNextProc = q_peek(&proc_queue);
            q_remove(&proc_queue);
        }
        else
        {
            nNextProc = nRunningProc;
        }
    }
    else
    if (nPolicy == POLICY_PSJF || nPolicy == POLICY_SJF)
    {
        for (i=0; i<nTotalPcb; ++i)
        {
            if (pPcb[ i ].nPid == -1 || pPcb[ i ].nExecTime == 0)
                continue;

            if (nNextProc == -1 || pPcb[ i ].nExecTime < pPcb[ nNextProc].nExecTime)
            {
                nNextProc = i;
            }
        }
    }

    return nNextProc;
}

void DoScheduling(struct EasyPCB *pPcb, int nTotalPcb, int nPolicy)
{
    int i;
    int nPid = getpid();
    
    q_init(&proc_queue);

    if (AssignProcessToCPU(nPid, CPU_FOR_PARENT) == -1)
    {
        perror("Fail to AssignProcessToCPU.\n");
    }

    if (WakeupProcess(nPid) == -1)
    {
        perror("Fail to WakeupProcess.\n");
    }

    int nFinished = 0;

    while (1)
    {
        if (nRunningProc != -1 && pPcb[ nRunningProc ].nExecTime == 0)
        {
            waitpid(pPcb[ nRunningProc ].nPid, NULL, 0);
            pPcb[ nRunningProc ].nPid = -1;
            nRunningProc = -1;
            nFinished++;

            if (nFinished == nTotalPcb)
                break;
        }

        for (i=0; i<nTotalPcb; ++i)
        {
            if (pPcb[ i ].nReadyTime == nUnitTime)
            {
                pPcb[ i ].nPid = LaunchProcess(&pPcb[ i ]);
                q_insert(&proc_queue,i);
                BlockProcess(pPcb[ i ].nPid);
            }
        }

        // Determine the next process to launch. //
        int nNextProc;
        if (nRunningProc != -1 && (nPolicy == POLICY_FIFO || nPolicy == POLICY_SJF))
        {
            nNextProc = nRunningProc;
        }
        else
        {
            nNextProc = SelectNextProcess(pPcb, nTotalPcb, nPolicy);
        }
        
        if (nNextProc != -1)
        {
            if (nRunningProc != nNextProc)      // context switching //
            {
                WakeupProcess(pPcb[ nNextProc ].nPid);
                BlockProcess(pPcb[ nRunningProc ].nPid);
                nRunningProc  = nNextProc;
                nLastUnitTime = nUnitTime;
            }
        }

        RUN_UINT_TIME();
        if (nRunningProc != -1)
        {
            pPcb[ nRunningProc ].nExecTime--;
        }
        nUnitTime++;
    }
}