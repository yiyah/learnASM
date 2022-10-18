#ifndef __PROTECT_H_
#define __PROTECT_H_

typedef struct s_descriptor
{
    u16 limit_low;
    u16 base_low;
    u8  base_mid;
    u8  attr1;              /* TYPE(4) S(1) DPL(2) P(1) */
    u8  limit_high_attr2;   /* LIMIT(4) AVL(1) 0(1) D/B(1) G(1) */
    u8  base_high;
}DESCRIPTOR;

typedef struct s_gate
{
    u16 offset_low;
    u16 selector;
    u8  dcount;             /* only use in call gate */
    u8  attr;
    u16 offset_high;
}GATE;

typedef struct s_tss {
    u32 backlink;
    u32 esp0;   /* stack pointer to use during interrupt */
    u32 ss0;    /*   "   segment  "  "    "        "     */
    u32 esp1;
    u32 ss1;
    u32 esp2;
    u32 ss2;
    u32 cr3;
    u32 eip;
    u32 flags;
    u32 eax;
    u32 ecx;
    u32 edx;
    u32 ebx;
    u32 esp;
    u32 ebp;
    u32 esi;
    u32 edi;
    u32 es;
    u32 cs;
    u32 ss;
    u32 ds;
    u32 fs;
    u32 gs;
    u32 ldt;
    u16 trap;
    u16 iobase; /* I/O位图基址大于或等于TSS段界限，就表示没有I/O许可位图 */
}TSS;


/* GDT */
/* index of descriptor */
#define INDEX_DUMMY     0   // `.
#define INDEX_FLAT_C    1   //  | determined in loader.asm
#define INDEX_FLAT_RW   2   //  |
#define INDEX_VIDEO     3   // /
#define INDEX_TSS       4
#define INDEX_LDT_FIRST 5
/* selector */
#define SELECTOR_DUMMY      0           // `.
#define SELECTOR_FLAT_C     0x08        //  | determined in loader.asm
#define SELECTOR_FLAT_RW    0x10        //  |
#define SELECTOR_VIDEO      (0x18+3)    // /  RPL = 3
#define SELECTOR_TSS        0x20        // TSS
#define SELECTOR_LDT_FIRST  0x28        // LDT

#define SELECTOR_KERNEL_CS  SELECTOR_FLAT_C
#define SELECTOR_KERNEL_DS  SELECTOR_FLAT_RW
#define SELECTOR_KERNEL_GS  SELECTOR_VIDEO

/* 每个任务有一个单独的 LDT, 每个 LDT 中的描述符个数: */
#define LDT_SIZE        2
/* descriptor indices in LDT */
#define INDEX_LDT_C     0
#define INDEX_LDT_RW    1

/* 选择子类型值说明 */
/* 其中, SA_ : Selector Attribute */
#define SA_RPL_MASK     0xFFFC
#define SA_RPL0         0
#define SA_RPL1         1
#define SA_RPL2         2
#define SA_RPL3         3

#define SA_TI_MASK      0xFFFB
#define SA_TIG          0       /* Table Indicator */
#define SA_TIL          4

/* RPL */
#define RPL_KRNL        SA_RPL0
#define RPL_TASK        SA_RPL1
#define RPL_USER        SA_RPL3

/* 描述符类型值说明 */
#define DA_32           0x4000  /* 32 位段 */
#define DA_LIMIT_4K     0x8000  /* 段界限粒度为 4K 字节 */
#define DA_DPL0         0x00    /* DPL = 0 */
#define DA_DPL1         0x20    /* DPL = 1 */
#define DA_DPL2         0x40    /* DPL = 2 */
#define DA_DPL3         0x60    /* DPL = 3 */
/* 存储段描述符类型值说明 */
#define DA_DR           0x90    /* 存在的只读数据段类型值 */
#define DA_DRW          0x92    /* 存在的可读写数据段属性值 */
#define DA_DRWA         0x93    /* 存在的已访问可读写数据段类型值 */
#define DA_C            0x98    /* 存在的只执行代码段属性值 */
#define DA_CR           0x9A    /* 存在的可执行可读代码段属性值 */
#define DA_CCO          0x9C    /* 存在的只执行一致代码段属性值 */
#define DA_CCOR         0x9E    /* 存在的可执行可读一致代码段属性值 */
/* 系统段描述符类型值说明 */
#define DA_LDT          0x82    /* 局部描述符表段类型值 */
#define DA_TaskGate     0x85    /* 任务门类型值 */
#define DA_386TSS       0x89    /* 可用 386 任务状态段类型值 */
#define DA_386CGate     0x8C    /* 386 调用门类型值 */
#define DA_386IGate     0x8E    /* 386 中断门类型值 */
#define DA_386TGate     0x8F    /* 386 陷阱门类型值 */

/* IDT interrupt vector */
#define INT_VECTOR_DIVIDE       0x0
#define INT_VECTOR_DEBUG        0x1
#define INT_VECTOR_NMI          0x2
#define INT_VECTOR_BREAKPOINT   0x3
#define INT_VECTOR_OVERFLOW     0x4
#define INT_VECTOR_BOUNDS       0x5
#define INT_VECTOR_INVAL_OP     0x6
#define INT_VECTOR_COPROC_NOT   0x7
#define INT_VECTOR_DOUBLE_FAULT 0x8
#define INT_VECTOR_COPROC_SEG   0x9
#define INT_VECTOR_INVAL_TSS    0xA
#define INT_VECTOR_SEG_NOT      0xB
#define INT_VECTOR_STACK_FAULT  0xC
#define INT_VECTOR_PROTECTION   0xD
#define INT_VECTOR_PAGE_FAULT   0xE
#define INT_VECTOR_COPROC_ERR   0x10

/* 8259A interrupt vector */
#define INT_VECTOR_IRQ0 0x20
#define INT_VECTOR_IRQ8 0x28

/* SYSTEM CALL */
#define INT_VECTOR_SYS_CALL     0x90

#endif /* __PROTECT_H_ */
