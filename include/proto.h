//////////////////////////////////////////////////////////////////////////////////////
#define MAX_QUEUE_SIZE 10
typedef struct queue  
{    
    int front;  
    int rear;  
    int queue_array[MAX_QUEUE_SIZE] ;  
      
}SqQueue;  



/* klib.asm */
PUBLIC void	out_byte(u16 port, u8 value);
PUBLIC u8	in_byte(u16 port);
PUBLIC void	disp_str(char * info);
PUBLIC void	disp_color_str(char * info, int color);

/* protect.c */
PUBLIC void	init_prot();
PUBLIC u32	seg2phys(u16 seg);

/* klib.c */
PUBLIC void	delay(int time);

/* kernel.asm */
void restart();

/* main.c */
void TestA();
void TestB();
void TestC();
void TestD();
void TestE();

/* i8259.c */
PUBLIC void put_irq_handler(int irq, irq_handler handler);
PUBLIC void spurious_irq(int irq);

/* clock.c */
PUBLIC void clock_handler(int irq);


/* 以下是系统调用相关 */

/* proc.c */
PUBLIC  int     sys_get_ticks();        /* sys_call */
PUBLIC	void	sys_process_sleep(int i);
PUBLIC  void    sys_disp_str(char* str);
PUBLIC  void    sys_sem_p(int ID,int p,SqQueue* s);
PUBLIC  void    sys_sem_v(int ID,SqQueue* s);
PUBLIC	void 	sys_color_output(int color,char* str);

/* syscall.asm */
PUBLIC  void    sys_call();             /* int_handler */
PUBLIC  int     get_ticks();
PUBLIC  void    process_sleep(int i);
PUBLIC  void    my_disp_str(char* str);
PUBLIC  void    sem_p(int ID,int p,SqQueue* s);
PUBLIC  void    sem_v(int ID,SqQueue* s);

