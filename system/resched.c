/* resched.c - resched, resched_cntl */

#include <xinu.h>

struct	defer	Defer;

/*------------------------------------------------------------------------
 *  resched  -  Reschedule processor to highest priority eligible process
 *------------------------------------------------------------------------
 */
void	resched(void)		/* Assumes interrupts are disabled	*/
{
	struct procent *ptold;	/* Ptr to table entry for old process	*/
	struct procent *ptnew;	/* Ptr to table entry for new process	*/
	struct procent *prptrrl;
	/* If rescheduling is deferred, record attempt and return */

	if (Defer.ndefers > 0) {
		Defer.attempt = TRUE;
		return;
	}

	/* Point to process table entry for the current (old) process */

	ptold = &proctab[currpid];
	int  pid_temp,pid_min;
	uint32 minTC;
	int flag;

	if (ptold->prstate == PR_CURR) {  /* Process remains eligible */
		if (ptold->prprio > firstkey(readylist)) {
			return;
		}

		/* Old process will no longer remain current */
	
		if(ptold->usr_proc==1){
	/*Compare TC of procs in ready list to currently running proc*/
			pid_temp=firstid(readylist);
			minTC= ptold->TC;
			while(pid_temp!=queuetail(readylist)){
				prptrrl=&proctab[pid_temp];
				if(prptrrl->TC<minTC){		
					minTC=prptrrl->TC;
					pid_min=pid_temp;
					flag=1;
				}
			
				pid_temp=queuetab[pid_temp].qnext;
			}
			
			if(flag==1){      
			/*We know that some proc in ready list has TC < currproc->TC*/
				ptold->prstate=PR_READY;
				if(ptold->usr_proc==1){
				kprintf("P%d-running::%d-%d\n",currpid,ptold->cr_time,ctr1000);
				}
				insert(currpid,readylist,ptold->prprio);
				/*Force context switch*/
				currpid=pid_min;
				getitem(currpid);					
				ptnew=&proctab[currpid];
				ptnew->prstate=PR_CURR;
				ptnew->cr_time=ctr1000;
				preempt=QUANTUM;
				ctxsw(&ptold->prstkptr, &ptnew->prstkptr);
				return;
			}
			else
				return;

		
		}

		ptold->prstate = PR_READY;
		insert(currpid, readylist, ptold->prprio);
	}

	/* Force context switch to highest priority ready process */

	currpid = dequeue(readylist);
	ptnew = &proctab[currpid];
	ptnew->prstate = PR_CURR;
	preempt = QUANTUM;		/* Reset time slice for process	*/
	ctxsw(&ptold->prstkptr, &ptnew->prstkptr);

	/* Old process returns here when resumed */

	return;
}

/*------------------------------------------------------------------------
 *  resched_cntl  -  Control whether rescheduling is deferred or allowed
 *------------------------------------------------------------------------
 */
status	resched_cntl(		/* Assumes interrupts are disabled	*/
	  int32	defer		/* Either DEFER_START or DEFER_STOP	*/
	)
{
	switch (defer) {

	    case DEFER_START:	/* Handle a deferral request */

		if (Defer.ndefers++ == 0) {
			Defer.attempt = FALSE;
		}
		return OK;

	    case DEFER_STOP:	/* Handle end of deferral */
		if (Defer.ndefers <= 0) {
			return SYSERR;
		}
		if ( (--Defer.ndefers == 0) && Defer.attempt ) {
			resched();
		}
		return OK;

	    default:
		return SYSERR;
	}
}
