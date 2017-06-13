/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            main.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "string.h"
#include "proc.h"
#include "global.h"

SqQueue S;
SqQueue Init_Queue()            /*  队列初始化  */  
{      
    SqQueue S;  
    S.front=S.rear=0;    
    return (S);  
}  
int push(SqQueue *S,int e)      /*  使数据元素e进队列成为新的队尾  */  
{    
    (*S).queue_array[(*S).rear]=e;    /* e成为新的队尾  */  
    (*S).rear++ ;                     /*  队尾指针加1  */  
}  
int pop(SqQueue *S,int *e )     /*弹出队首元素*/  
{    
    *e=(*S).queue_array[(*S).front] ;    
    (*S).front++;    
}   


/*======================================================================*
                            kernel_main
 *======================================================================*/
PUBLIC int kernel_main()
{
	TASK*		p_task		= task_table;
	PROCESS*	p_proc		= proc_table;
	char*		p_task_stack	= task_stack + STACK_SIZE_TOTAL;
	u16		selector_ldt	= SELECTOR_LDT_FIRST;
	int i;
	for (i = 0; i < NR_TASKS; i++) {
		strcpy(p_proc->p_name, p_task->name);	// name of the process
		p_proc->pid = i;			// pid

		p_proc->ldt_sel = selector_ldt;

		memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3],
		       sizeof(DESCRIPTOR));
		p_proc->ldts[0].attr1 = DA_C | PRIVILEGE_TASK << 5;
		memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3],
		       sizeof(DESCRIPTOR));
		p_proc->ldts[1].attr1 = DA_DRW | PRIVILEGE_TASK << 5;
		p_proc->regs.cs	= ((8 * 0) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.ds	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.es	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.fs	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.ss	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK)
			| SA_TIL | RPL_TASK;
		p_proc->regs.gs	= (SELECTOR_KERNEL_GS & SA_RPL_MASK)
			| RPL_TASK;

		p_proc->regs.eip = (u32)p_task->initial_eip;
		p_proc->regs.esp = (u32)p_task_stack;
		p_proc->regs.eflags = 0x1202; /* IF=1, IOPL=1 */

		p_task_stack -= p_task->stacksize;
		p_proc++;
		p_task++;
		selector_ldt += 1 << 3;
		proc_table[i].ticks = proc_table[i].isWait = 0;
	}


	k_reenter = 0;
	ticks = 0;
	
	waiting = 0;
	mutex = 1;
	barbers = 0;
	customers = 0;
	CHAIRS = 3;
	cusid = 0;

	p_proc_ready	= proc_table;

        /* 初始化 8253 PIT */
        out_byte(TIMER_MODE, RATE_GENERATOR);
        out_byte(TIMER0, (u8) (TIMER_FREQ/HZ) );
        out_byte(TIMER0, (u8) ((TIMER_FREQ/HZ) >> 8));

        put_irq_handler(CLOCK_IRQ, clock_handler); /* 设定时钟中断处理程序 */
        enable_irq(CLOCK_IRQ);                     /* 让8259A可以接收时钟中断 */

	disp_pos=0;
	for(i=0;i<80*25;i++){
		disp_str(" ");
	}
	disp_pos=0;

	S=Init_Queue();

	restart();

	while(1){}
}



/*======================================================================*
                               TestA
 *======================================================================*/
void TestA()
{
	int i = 0;
	while (1) {
		milli_delay(10);
	}
}

/*======================================================================*
                               TestB
 *======================================================================*/
void TestB()
{
	int i = 0x1000;
	while(1){
		sys_sem_p(CUSTOMERS,1,&S);
		sys_sem_p(MUTEX,1,&S);
		waiting--;
		sys_sem_v(BARBERS,&S);
		sys_sem_v(MUTEX,&S);
		my_milli_delay(3000);
		sys_color_output(0x07,"WAKE");
		my_disp_str("\n");

	}
}

/*======================================================================*
                               TestC
 *======================================================================*/
void TestC()
{	milli_delay(3000);
	int i = 0x2000;
	while(1){
	cusid = cusid+1;
	char *temp;
	itoa(temp,cusid);
	sys_color_output(0x02,temp);
	sys_color_output(0x02," CUS COME");
	my_disp_str("\n");
	sys_sem_p(MUTEX,2,&S);
	if(waiting<CHAIRS){
		waiting++;
		sys_sem_v(CUSTOMERS,&S);
		sys_sem_v(MUTEX,&S);
		sys_color_output(0x02,"A WAIT");
		my_disp_str("\n");
		sys_sem_p(BARBERS,2,&S);
		sys_color_output(0x02,"A CUT");
		my_disp_str("\n");
			
	}
	else{
		sys_sem_v(MUTEX,&S);
		sys_color_output(0x02,"LEAVE");
		my_disp_str("\n");	
	}
	milli_delay(3000);
	}
}

/*======================================================================*
                               TestD
 *======================================================================*/
void TestD()
{	milli_delay(3000);

	int i = 0x3000;
	while(1){
	cusid = cusid+1;
	char temp[24];
	itoa(temp,cusid);
	sys_color_output(0x03,temp);
	sys_color_output(0x03," CUS COME");
	my_disp_str("\n");
	sys_sem_p(MUTEX,3,&S);
	if(waiting<CHAIRS){
		waiting++;
		sys_sem_v(CUSTOMERS,&S);
		sys_sem_v(MUTEX,&S);
		sys_color_output(0x03,"B WAIT");
		my_disp_str("\n");
		sys_sem_p(BARBERS,3,&S);
		sys_color_output(0x03,"B CUT");
		my_disp_str("\n");	
		
	}
	else{
		sys_sem_v(MUTEX,&S);
		sys_color_output(0x03,"LEAVE");
		my_disp_str("\n");		
	}
	milli_delay(3000);
	}

	
}

/*======================================================================*
                               TestE
 *======================================================================*/
void TestE()
{
	milli_delay(3000);
	int i = 0x4000;
	while(1){
	cusid = cusid+1;
	char * temp;
	itoa(temp,cusid);
	sys_color_output(0x04,temp);
	sys_color_output(0x04," CUS COME");
	my_disp_str("\n");
	sys_sem_p(MUTEX,4,&S);
	if(waiting<CHAIRS){
		waiting++;
		sys_sem_v(CUSTOMERS,&S);
		sys_sem_v(MUTEX,&S);
		sys_color_output(0x04,"C WAIT");
		my_disp_str("\n");
		sys_sem_p(BARBERS,4,&S);
		sys_color_output(0x04,"C CUT");
		my_disp_str("\n");
			
	}
	else{
		sys_sem_v(MUTEX,&S);		
		sys_color_output(0x04,"LEAVE");
		my_disp_str("\n");	
	}
	milli_delay(3000);
	}
	
	
}

