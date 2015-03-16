# lec5 SPOC思考题


NOTICE
- 有"w3l1"标记的题是助教要提交到学堂在线上的。
- 有"w3l1"和"spoc"标记的题是要求拿清华学分的同学要在实体课上完成，并按时提交到学生对应的git repo上。
- 有"hard"标记的题有一定难度，鼓励实现。
- 有"easy"标记的题很容易实现，鼓励实现。
- 有"midd"标记的题是一般水平，鼓励实现。


## 个人思考题
---

请简要分析最优匹配，最差匹配，最先匹配，buddy systemm分配算法的优势和劣势，并尝试提出一种更有效的连续内存分配算法 (w3l1)
```
  + 采分点：说明四种算法的优点和缺点
  - 答案没有涉及如下3点；（0分）
  - 正确描述了二种分配算法的优势和劣势（1分）
  - 正确描述了四种分配算法的优势和劣势（2分）
  - 除上述两点外，进一步描述了一种更有效的分配算法（3分）
 ```
- [x]  

>   最优匹配：优势：相对简单，容易操作，由于每一次找的都是能够满足条件的区域中最小的，其产生的剩余也是最小的，也就是说外部碎片的大小被降低
					了。
			  劣势：产生的外部碎片虽然比较小，但也正因为如此导致这些碎片重新被利用的概率降低，这些碎片其实是一种资源的浪费，另外，当进程结
					束的需要释放资源的时候，因为最佳匹配维护的是一个按照大小排序的空间链表，所以释放之后需要查找地址上临近的空间，这实际上
					需要重新搜索一遍。
	最差匹配：优势：适用于中等大小分配，因为其第一次剩下的空间被再次分配的可能性大，相对于最优匹配产生小碎片的可能性会降低。
			  劣势：跟最优匹配相同，最小匹配也是维护了一个按照大小排序的链表（只是大小顺序不同，最差按降序，最优按升序），因此在资源释放阶
					段会比较慢，同时外部碎片的产生也是不可避免的；另外由于每一次都是找最大的，导致以后给需要大空间的进程分配会比较困难，
	最先匹配；优势：首先是实现简单，因为查找空闲分区链表找到第一个合适的就分配给当前进程，这样做的另一个好处是在低地址处匹配的可能性较大，
					所以高地址空间留下的内存区域比较大，给需要大量空间的进程留下空间。
			  劣势：因为符合条件的空闲内存都是大于进程所需要的内存，所以分配之后会产生外部碎片，而且从概率上看，大块内存需要尽量向链表后部
					找，所以分配大块内存较慢。
	buddy system：优势：使用二维数组存储空闲块，不仅存有地址信息，还有大小信息，释放进程的时候比较快；有效利用空间，防止较大外部碎片的产生
				  劣势：操作繁琐，数组维护比较麻烦。

## 小组思考题

请参考ucore lab2代码，采用`struct pmm_manager` 根据你的`学号 mod 4`的结果值，选择四种（0:最优匹配，1:最差匹配，2:最先匹配，3:buddy systemm）
分配算法中的一种或多种，在应用程序层面(可以用python,ruby,C++，C，LISP等高语言)来实现，给出你的设思路，并给出测试用例。 (spoc)

--- 
>学号276，做的是最优匹配，用一个链表存储空闲的区域，测试用例使用了十个进程，分别分配内存和释放，观察结果，以下是源代码和运行结果：
	#include <iostream>

using namespace std;

//整个内存空间为1M

struct block{
	int begin;//起始地址
	int size;//块大小
	block* pre;//前驱
	block* sub;//后继
};

struct proccess{
	int id;//进程id
	int begin;//起始地址
	int size;//占用块大小
	int type;//in或out0/1
};

proccess os[20];

void construct(int turn, int vid, int vsize, int vtype){
	os[turn].id = vid;
	if(vtype == 0)
		os[turn].size = vsize;
	os[turn].type = vtype;
}

block* bhead;

block* init(){
	bhead = new block;
	bhead->pre = NULL;
	bhead->sub = NULL;
	bhead->begin = 0;
	bhead->size = 1023;
	
	return bhead; 
}

void print(){//打印空闲区间表
	//cout << "0 " << head->size << endl;
	int i = 0;
	block* it = new block;
	it = bhead;
	do{
	    system("pause");
		cout << i << " " << it->begin << " " << it->size << endl;
		if(it->sub)
			it = it->sub;
		else
			break;
		i ++;
	}while(it->sub);
}

void sort(block* it, int type){
	if (type == 0){
		while(it->pre){
		    cout << "hh" << endl;
			if(it->size < it->pre->size){//分配
				it->pre->sub = it->sub;
				it->sub = it->pre;
				
				it->pre = it->sub->pre;
				it->sub->pre = it;
				
				int temp1 = it->begin;
				int temp2 = it->size;
				it->begin = it->sub->begin;
				it->size = it->sub->size;
				it->sub->begin = temp1;
				it->sub->size = temp2;
			}
			else{
			   break;
            }    
			it = it->pre;
		}
	}
	else{
		while(it->sub){
			if(it->size > it->sub->size){//释放
				it->sub->pre = it->pre;
				it->pre = it->sub;
				
				it->sub = it->pre->sub;
				it->pre->sub = it;
				
				int temp1 = it->begin;
				int temp2 = it->size;
				it->begin = it->pre->begin;
				it->size = it->pre->size;
				it->pre->begin = temp1;
				it->pre->size = temp2;
			}
			else{
			   break;			
            }
            it = it->sub;
		}
	}
}


int allocation(int psize){//返回进程空间开始地址
	block* it = new block;
	it = bhead;
	do{
		if(it->size >= psize){
			int temp = it-> begin;
			it->begin = it->begin + psize;
			it->size = it->size - psize;
			sort(it, 0);
			return temp;
		}
	}while(it->sub);
}

void free(int pbegin, int psize){//释放进程
	//int temp = 
	block* it = new block;
	it = bhead;
	block* temp1;
    block* temp2;
	temp1 = new block;
	temp2 = new block;
	int i = 0;
	do{
		if(it->begin + it->size == pbegin){//前有
			i ++;
			temp1 = it;
			if(it->sub && (it->sub->begin == pbegin + psize)){//后有
				i += 2;
				temp2= it->sub;
			}
		}
	}while(it->sub);
	
	cout << "                          " << i << endl;
	switch(i){
		case 0:
			it->begin = pbegin;
			it->size = psize;
			
			it->pre = bhead;
			if(bhead->sub){
			   	it->sub = bhead->sub;
                bhead->sub->pre = it;
                bhead->sub = it;	
			}
			else
				it->sub = NULL;
			sort(it, 0);
			sort(it, 1);
			print();
			break;
		case 1:
			temp1->size = temp1->size + psize;
			sort(temp1, 1);
			break;
		case 2:
			temp2->size = temp2->size + psize;
			temp2->begin = temp2->begin - psize;
			sort(temp2, 1);
			break;
		case 3:
			temp1->size = temp1->size + psize + temp2->size;
			sort(temp1, 1);
			break;
		default:
			break;
	}
	delete it;
	delete temp1;
	delete temp2;
}

void control(){
	for (int i = 0; i < 20; i ++){
		cout << i << endl;
		if(os[i].type == 0)
			os[i].begin = allocation(os[i].size);//分配
		else if(os[i].type == 1){
			for (int j = 0; j < 20; j ++){
				if(os[j]. id == os[i]. id){
					cout << "             " << j << endl;
					free(os[j].begin, os[j].size);//释放
					break;
				}
			}
		}
		print();
	}
}

int main(){
	construct(0, 0, 100, 0);construct(1, 1, 64, 0);construct(2, 2, 200, 0);construct(3, 3, 400, 0);
	construct(4, 1, 0, 1);construct(5, 4, 32, 0);construct(6, 2, 0, 1);construct(7, 5, 80, 0);
	construct(8, 0, 0, 1);construct(9, 6, 60, 0);construct(10, 3, 0, 1);construct(11, 7, 120, 0);
	construct(12, 8, 200, 0);construct(13, 6, 0, 1);construct(14, 9, 70, 0);construct(15, 7, 0, 1);
	construct(16, 9, 0, 1);construct(17, 5, 0, 1);construct(18, 4, 0, 1);construct(19, 8, 100, 1);
	init();
	control();
	return 0;
}
运行结果：
（格式为编号、开始地址、规模）
0 100 924
0 164 860
0 364 660
0 100 64 1 400 60
0 132 32 1 400 60
0 312 232 1 400 60
。。。

## 扩展思考题

阅读[slab分配算法](http://en.wikipedia.org/wiki/Slab_allocation)，尝试在应用程序中实现slab分配算法，给出设计方案和测试用例。

## “连续内存分配”与视频相关的课堂练习

### 5.1 计算机体系结构和内存层次
MMU的工作机理？

- [x]  

>  http://en.wikipedia.org/wiki/Memory_management_unit

L1和L2高速缓存有什么区别？

- [x]  

>  http://superuser.com/questions/196143/where-exactly-l1-l2-and-l3-caches-located-in-computer
>  Where exactly L1, L2 and L3 Caches located in computer?

>  http://en.wikipedia.org/wiki/CPU_cache
>  CPU cache

### 5.2 地址空间和地址生成
编译、链接和加载的过程了解？

- [x]  

>  

动态链接如何使用？

- [x]  

>  


### 5.3 连续内存分配
什么是内碎片、外碎片？

- [x]  

>  

为什么最先匹配会越用越慢？

- [x]  

>  

为什么最差匹配会的外碎片少？

- [x]  

>  

在几种算法中分区释放后的合并处理如何做？

- [x]  

>  

### 5.4 碎片整理
一个处于等待状态的进程被对换到外存（对换等待状态）后，等待事件出现了。操作系统需要如何响应？

- [x]  

>  

### 5.5 伙伴系统
伙伴系统的空闲块如何组织？

- [x]  

>  

伙伴系统的内存分配流程？

- [x]  

>  

伙伴系统的内存回收流程？

- [x]  

>  

struct list_entry是如何把数据元素组织成链表的？

- [x]  

>  



