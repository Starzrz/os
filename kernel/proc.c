
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               proc.c
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

extern SqQueue Init_Queue() ;           /*  队列初始化  */  

extern  int push(SqQueue *S,int e) ;     /*  使数据元素e进队列成为新的队尾  */  
 
extern int pop(SqQueue *S,int *e ) ;    /*弹出队首元素*/  

void next();

/*======================================================================*
                              schedule
 *======================================================================*/
PUBLIC void schedule()
{
	PROCESS* p;

	for (p = proc_table; p < proc_table+NR_TASKS; p++) {
		if(p->isWait){
			continue;
		}
		if(p->ticks){
			p->ticks--;
		}
	}
 
	p_proc_ready++;
	while(1){
		if(p_proc_ready->isWait){
			p_proc_ready++;
		}
		if (p_proc_ready >= proc_table + NR_TASKS) {
			p_proc_ready = proc_table;
		}
		if(p_proc_ready->ticks){
			p_proc_ready++;
		} else {
			break;
		}
	}

}

/*======================================================================*
                           sys_get_ticks
 *======================================================================*/
PUBLIC int sys_get_ticks()
{
	return ticks;
}

/*======================================================================*
                           sys_process_sleep
 *======================================================================*/
PUBLIC void sys_process_sleep(int i)
{
	p_proc_ready->ticks = i*HZ/1000;
}

/*======================================================================*
                           sys_disp_str
 *======================================================================*/
PUBLIC void sys_disp_str(char* str)
{
	disp_str(str);
}
/*======================================================================*
                           sys_sem_p
 *======================================================================*/
PUBLIC void sys_sem_p(int ID,int p,SqQueue* s)
{
	if(ID == MUTEX){
		--mutex;
		if(mutex<0){
			p_proc_ready->isWait = 1;
			
			push(s,p);
			while(p_proc_ready->isWait){}
		}
	} else if(ID == BARBERS){
		--barbers;
		if(barbers<0){
			p_proc_ready->isWait = 1;
		
			push(s,p);
			while(p_proc_ready->isWait){}
		}
	} else if(ID == CUSTOMERS){
		--customers;
		if(customers<0){
			p_proc_ready->isWait = 1;
			
			push(s,p);
			while(p_proc_ready->isWait){}
		}
	}
}
/*======================================================================*
                           sys_sem_v
 *======================================================================*/
PUBLIC void sys_sem_v(int ID,SqQueue* s)
{
	
	if(ID == MUTEX){
		++mutex;
		if(mutex <= 0) {
			int t=0;
			pop(s,&t);
			proc_table[t].isWait = 0;
		}
	} else if(ID == BARBERS){
		++barbers;
		if(barbers <= 0) {
			int t=0;
			pop(s,&t);
			proc_table[t].isWait = 0;
		}
	} else if(ID == CUSTOMERS){
		++customers;
		if(customers <= 0) {
			int t=0;
			pop(s,&t);
			proc_table[t].isWait = 0;
		}
	}
}
/*======================================================================*
                           sys_color_out
 *======================================================================*/
PUBLIC void sys_color_output(int color,char* str ){
	disp_color_str(str,color);
}

