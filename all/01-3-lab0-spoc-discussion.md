# lab0 SPOC思考题

## 个人思考题

---

能否读懂ucore中的AT&T格式的X86-32汇编语言？请列出你不理解的汇编语言。
- [x]  

>  http://www.imada.sdu.dk/Courses/DM18/Litteratur/IntelnATT.htm
基本的命令都能够理解，应该没有问题

虽然学过计算机原理和x86汇编（根据THU-CS的课程设置），但对ucore中涉及的哪些硬件设计或功能细节不够了解？
- [x]  

>  不懂的居多，中断和实虚地址的转换了解一些。对于文件系统、内存管理等不太清楚。


哪些困难（请分优先级）会阻碍你自主完成lab实验？
- [x]  

>  视频讲解不清晰的时候不能随时提问，大家都不会自己有问题不能及时解决

如何把一个在gdb中或执行过程中出现的物理/线性地址与你写的代码源码位置对应起来？
- [x]  

>   逻辑地址通过页机制映射跟线性地址联系起来，通过段机制映射跟物理地址联系起来


了解函数调用栈对lab实验有何帮助？
- [x]  

> 了解函数调用栈是如何工作的能使我们更加了解函数调用的过程，而lab实验基本上就是一
些函数调用的实验，这样应该对效率的提高有帮助。

你希望从lab中学到什么知识？
- [x]  

>   深入了解操作系统内部运行机制，了解各层次之间的关系和功能，希望能够对文件系统人士深点。

---

## 小组讨论题

---

搭建好实验环境，请描述碰到的困难和解决的过程。
- [x]  

> 搭建过程按照视频介绍的过程基本上没有遇到太大的问题，就是发现给的地址好乱，最后是问助教才
知道下哪一个，没有直接将文件上传到网络学堂方便。

熟悉基本的git命令行操作命令，从github上
的 http://www.github.com/chyyuu/ucore_lab 下载
ucore lab实验
- [x]  

> 已经下载

尝试用qemu+gdb（or ECLIPSE-CDT）调试lab1
- [x]   

> 调试成功

对于如下的代码段，请说明”：“后面的数字是什么含义
```
/* Gate descriptors for interrupts and traps */
struct gatedesc {
    unsigned gd_off_15_0 : 16;        // low 16 bits of offset in segment
    unsigned gd_ss : 16;            // segment selector
    unsigned gd_args : 5;            // # args, 0 for interrupt/trap gates
    unsigned gd_rsv1 : 3;            // reserved(should be zero I guess)
    unsigned gd_type : 4;            // type(STS_{TG,IG32,TG32})
    unsigned gd_s : 1;                // must be 0 (system)
    unsigned gd_dpl : 2;            // descriptor(meaning new) privilege level
    unsigned gd_p : 1;                // Present
    unsigned gd_off_31_16 : 16;        // high bits of offset in segment
};
```

- [x]  

> 这表示这个无符号整数的位域，单位是bit，表示它是一个占据16bit的无符号整数，这是因为不同的
系统对数字位的定义不同，指定了位可以增加可移植性。

对于如下的代码段，
```
#define SETGATE(gate, istrap, sel, off, dpl) {            \
    (gate).gd_off_15_0 = (uint32_t)(off) & 0xffff;        \
    (gate).gd_ss = (sel);                                \
    (gate).gd_args = 0;                                    \
    (gate).gd_rsv1 = 0;                                    \
    (gate).gd_type = (istrap) ? STS_TG32 : STS_IG32;    \
    (gate).gd_s = 0;                                    \
    (gate).gd_dpl = (dpl);                                \
    (gate).gd_p = 1;                                    \
    (gate).gd_off_31_16 = (uint32_t)(off) >> 16;        \
}
```

如果在其他代码段中有如下语句，
```
unsigned intr;
intr=8;
SETGATE(intr, 0,1,2,3);
```
请问执行上述指令后， intr的值是多少？

- [x]  

> 65538

请分析 [list.h](https://github.com/chyyuu/ucore_lab/blob/master/labcodes/lab2/libs/list.h)内容中大致的含义，并能include这个文件，利用其结构和功能编写一个数据结构链表操作的小C程序
- [x]  

> list.h头文件集中定义了双链表（struct list_head结构体）的相关结构体和操作。
可以使用如下程序检测，首先使用new关键字构造三个节点，然后利用list_add函数将
这些节点连接成链表输出链表中存储的数据，观察运行结果
#include <iostream>
#include "list.h"
using namespace std;
int main()
{
     list_entry_t* node1 = new list_entry_t;
     list_entry_t* node2 = new list_entry_t;
     list_entry_t* node3 = new list_entry_t;
     
     list_init(node1);
     list_add(node1, node2);
     list_add(node2, node3);

     cout << list_prev(node3) << ' ' << elm2;
     system("pause");
     return 0;   
}

---

## 开放思考题

---

是否愿意挑战大实验（大实验内容来源于你的想法或老师列好的题目，需要与老师协商确定，需完成基本lab，但可不参加闭卷考试），如果有，可直接给老师email或课后面谈。
- [x]  

>  

---
