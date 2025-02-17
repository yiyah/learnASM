#include "type.h"
#include "const.h"
#include "protect.h"
#include "process.h"
#include "tty.h"
#include "console.h"
#include "proto.h"
#include "global.h"


PUBLIC u32      k_reenter;
PUBLIC PROCESS* p_proc_ready;
PUBLIC PROCESS  proc_tables[NR_TASKS+NR_PROCS];
PUBLIC u8       task_stack[STACK_SIZE_TOTAL];       /* include all process stack */


PUBLIC int get_ticks()
{
    MESSAGE msg;
    reset_msg(&msg);
    msg.type = GET_TICKS;
    send_recv(BOTH, TASK_SYS, &msg);
    return msg.RETVAL;
}

void TestA()
{
    int i = 0;

    while(1)
    {
        printf("<Ticks:%x>", get_ticks());
        //disp_str("A");
        milli_delay(200);
    }
}

void TestB()
{
    while(1)
    {
        printf("B ");
        //disp_str("B");
        milli_delay(200);
    }
}

void TestC()
{
    while(1)
    {
        //disp_str("C");
        printf("C ");
        milli_delay(200);
    }
}

PUBLIC void kernel_main()
{
    ClearScreen();      /* just for show cursor */
    disp_str("--------kernel main begins--------\n");

    PROCESS* p_proc = proc_tables;
    TASK*    p_task = task_table;
    char*    p_task_stack = task_stack + STACK_SIZE_TOTAL;      /* top of stack */
    u16      selector_ldt = SELECTOR_LDT_FIRST;
    int      i = 0;
    u8       privilege;
    u8       rpl;
    u32      eflags;
    int      prio;

    for(i = 0; i < NR_TASKS+NR_PROCS; i++) {
        if (i < NR_TASKS) {             /* task */
            p_task    = task_table + i;
            privilege = PRIVILEGE_TASK;
            rpl       = RPL_TASK;
            eflags    = 0x1202;         // IF=1, IOPL=1, bit 2 is always 1.
            prio      = 15;
        }            
        else {                          /* user process */
            p_task    = user_proc_table + (i - NR_TASKS);
            privilege = PRIVILEGE_USER;
            rpl       = RPL_USER;
            eflags    = 0x202;         // IF=1, bit 2 is always 1.
            prio      = 5;
        }

        strcpy(p_proc->p_name, p_task->name);
        p_proc->pid = i;

        /* init descriptor in LDT */
        p_proc->ldt_sel = selector_ldt;
        memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS>>3], sizeof(DESCRIPTOR));
        p_proc->ldts[0].attr1 = DA_C | privilege << 5;
        memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS>>3], sizeof(DESCRIPTOR));
        p_proc->ldts[1].attr1 = DA_DRW | privilege << 5;
        
        /* init process table */
        p_proc->regs.cs = (0 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl ;
        p_proc->regs.ds = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl ;
        p_proc->regs.es = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl ;
        p_proc->regs.fs = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl ;
        p_proc->regs.ss = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl ;
        p_proc->regs.gs = (SELECTOR_KERNEL_GS & SA_RPL_MASK) | rpl;
        p_proc->regs.eip = (u32)p_task->initial_eip;
        p_proc->regs.esp = (u32)p_task_stack;
        p_proc->regs.eflags = eflags;        // IF=1, IOPL=1, bit 2 is always 1.
        p_proc->nr_tty = 0;

        p_proc->p_flags = 0;
        p_proc->p_msg = 0;
        p_proc->p_recvfrom = NO_TASK;
        p_proc->p_sendto = NO_TASK;
        p_proc->has_int_msg = 0;
        p_proc->q_sending = 0;
        p_proc->next_sending = 0;

        p_proc->ticks = p_proc->priority = prio;
    
        /* for next init LDT */
        p_task_stack -= p_task->stacksize;
        p_proc++;
        p_task++;
        selector_ldt += 1 << 3;
    }

    proc_tables[NR_TASKS + 0].nr_tty = 0;   /* TestA */
    proc_tables[NR_TASKS + 1].nr_tty = 1;   /* TestB */
    proc_tables[NR_TASKS + 2].nr_tty = 1;   /* TestC */

    k_reenter = 0;              /* the first time will self-decrement */
    ticks = 0;

    p_proc_ready = proc_tables;

    init_clock();               /* init and open clock interrupt */
    init_keyboard();            /* init and open keyboard interrupt */

    restart();

    while(1){}
}
