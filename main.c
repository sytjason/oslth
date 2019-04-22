#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "vscheduler.h"


int main(int argc, char *argv[])
{
    char szSchedPolicy[ 256 ];
    int  nTotalPcb;
    int  nPolicy;
    int  i;
    struct EasyPCB *pPcb;

    if (argc < 2)
    {
        perror("invalide argc.");
        return -1;
    }

    FILE *fd = fopen(argv[ 1 ], "r");
    if (fd == NULL)
    {
        perror("fail to open.");
        return -1;
    }

    // Get the scheduling policy. //
    fscanf(fd, "%s", szSchedPolicy);
    if (strcmp(szSchedPolicy, "FIFO") == 0)
    {
        nPolicy = POLICY_FIFO;
    }
    else
    if (strcmp(szSchedPolicy, "RR") == 0)
    {
        nPolicy = POLICY_RR;
    }
    else
    if (strcmp(szSchedPolicy, "SJF") == 0)
    {
        nPolicy = POLICY_SJF;
    }
    else
    if (strcmp(szSchedPolicy, "PSJF") == 0)
    {
        nPolicy = POLICY_PSJF;
    }

    // Get the number of process. //
    fscanf(fd, "%d", &nTotalPcb);

    // Allocate enough memory for each pcb. //
    pPcb = (struct EasyPCB *) malloc (nTotalPcb * sizeof(struct EasyPCB));

    // Get process information. //
    for (i=0; i<nTotalPcb; ++i)
    {
        fscanf(fd, "%s %d %d", pPcb[ i ].szName, &(pPcb[ i ].nReadyTime), &(pPcb[ i ].nExecTime));
        // Set pid = -1 to imply not ready //
        pPcb[ i ].nPid = -1;
    }
    fclose(fd);

    DoScheduling(pPcb, nTotalPcb, nPolicy);

    free(pPcb);

    return 0;
}