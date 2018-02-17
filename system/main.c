/*  main.c  - main */

#include <xinu.h>

process	main(void)
{

	/* Run the Xinu shell */
	
	pid32 pid1,pid2,pid3;
	pid1=create_user_proc(timed_execution,1024,100,"fun_A",0);
	pid2=create_user_proc(timed_execution,1024,100,"fun_B",0);

	resume(pid1);
	resume(pid2);

	sleepms(20);

	pid3=resume(create_user_proc(timed_execution,1024,50,"fun_C",0));


	recvclr();
	//resume(create(shell, 8192, 50, "shell", 1, CONSOLE));

	/* Wait for shell to exit and recreate it */

	/*while (TRUE) {
		receive();
		sleepms(200);
		kprintf("\n\nMain process recreating shell\n\n");
		resume(create(shell, 4096, 20, "shell", 1, CONSOLE));
	}*/
	return OK;
    
}
