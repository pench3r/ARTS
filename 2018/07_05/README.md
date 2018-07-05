# ARTS 01 #
---

## Algorithm

#### problem: ####

> Given an array of integers, return indices of the two numbers such that they add up to a specific target.
> 
> You may assume that each input would have exactly one solution, and you may not use the same element twice.
> 
> Example:
> 
> Given nums = [2, 7, 11, 15], target = 9,
> 
> Because nums[0] + nums[1] = 2 + 7 = 9,
> return [0, 1].

#### thinking: ####

题目本质是配对找到`a + b = target`,并且输入的元素不存在重复。这样必定需要进行遍历来记录所有元素的信息，在第一次实现时直接嵌套2层for循环进行两次遍历，使用的数据结构为`vector`, 时间复杂度为`n的平方`. 

优化:将每个元素的索引与`target-元素的值`通过`hash表`建立`kv`的关系，使用`hash表`的优点为key唯一，搜索速度快。使用`c++11`中的`unordered_map`进行kv存储，我这里先进行了一次遍历初始化`hash表`,接着又遍历了一次`vector`,优化后效果显著。

PS：最后查看discuss中发现，直接通过一次遍历就可以达到目标，在遍历中先查看元素对应的`target-element`是否存在于hash表中，存在直接返回结果，不存在则插入到hash表中。由于这里有唯一解，假如结果为[i,j],当遍历到j时，i索引的相关信息肯定已经存入hash表中，这样实现更优雅理论上该解法速度更快。

#### soultion: ####

	class Solution {
	public:
	    vector<int> twoSum(vector<int>& nums, int target) {
	        unordered_map<int, int> indices = {};
	        for (int i=0; i < nums.size(); ++i) {
	            indices[target - nums[i]] = i;
	        }
	        for(int i=0; i < nums.size(); ++i) {
	            if (indices.count(nums[i]) && indices[nums[i]] != i) {
	                return {i, indices[nums[i]]};
	            } else {
	                indices.erase(nums[i]);
	            }
	        }
	        return {};
	    }
	};

#### 中间碰到的问题： ####

- 对于unordered_map和vector的使用不太了解，通过cppreference查询获取用法和语法
- 有些特殊情况在一开始没有考虑清楚，例如输入为[3, 2, 4]就会返回[0,0],添加特殊情况的处理，正常返回结果

## Review ##

原文链接： [http://www.avabodh.com/cxxin/namemangling.html](http://www.avabodh.com/cxxin/namemangling.html)

#### 理论知识掌握： ####

重新认识`name mangling`和`function overloading`,`c++`通过函数名、参数个数、参数类型、参数顺序来确定唯一的函数标识，并且不同的编译器可能对于name mangling策略有所不同。

name mangling pattern:

	_Z numberOfCharsInOriginalFunctionName OriginalFunctionName parameter_names_encoded

例子：

	void myFun();    Mangled name: _Z5myFunv
	void myFun(int a,char c);  Mangled name: _Z5myFunic

	char: c
	int: i
	long: l
	void: v
	array or pointer: P
	user defined type names: for class name myClass, it will be encoded as 7myClass

对于类和namespace有所不同：

	MyClass::MyFunction(int a);   Mangled name: _ZN7MyClass10MyFunctionEi

	NS::myClass::MyFunction(int a);  Mangled name: _ZN2NS7myClass10MyFunctionEi

并且GNU有个工具C++filt可以进行名字的解析和mangle, 这个不需要死记格式，只是需要了解有这样的方法可以进行确定函数名字的唯一性。

#### 应用场景1： ####

对于C++和C混合使用的源码文件，可以更容易理解：

libc.cpp

	int libFunc(int x){
		return ++x;
	}

use.cpp

	int libFunc(int x);

	int main() {
		printf("%d", libFunc(2));
		return 0;
	}

当两个文件都使用g++来编译和链接时，可以正常运行，libc.o会导入函数名为`_Z7libFunci`, 同时use.o会导入相同的函数名(由于都是用g++).

记录一下编译命令：

	g++ -c libc.cpp -o libc.o	# 综合预处理、编译、汇编
	g++ -c use.cpp -o use.o
	g++ use.o libc.o -o use		# 链接

但如果`libc.cpp`使用C编译器来编译,2个文件在链接时会报错.

因为`libc.cpp`导出的函数名会变成libFunc, 但是use.cpp还是引用的`_Z7libFunci`, C语言是没有`name mangling`一说.

解决办法是在C++中使用`extern "C"`来解决

use.cpp

	extern "C" int libFunc(int x);
	...

这样`use.cpp`导入的函数名就为`libFunc`，可以成功链接。

	gcc -c libc.cpp -o libc.o

#### 应用场景2: ####

当C和C++公用同一个头文件时，可能会造成不兼容。由于C中并没有`extern "C"`这样的关键字。因此在头文件定义时需要如下格式：

libc.h

	#ifdef __cplusplus
		extern "C" int libFunc(int x);
	#else
		int libFunc(int x);
	#endif

libc.cpp

	#include "libc.h"
	...

use.cpp
	
	#include "libc.h"
	...

分别编译，最后链接都可以正常工作。

#### 总结: ####

对于`extern "C"`, `__cplusplus`概念清晰了很多，不像以前是模糊的感觉。

## Technique ##

- https://www.ibm.com/support/knowledgecenter/ssw_i5_54/rzab6/xafunixsrv.htm （一些demo)
- http://osr507doc.sco.com/en/netguide/dusockT.datagram_code_samples.html  （一些demo)
- https://blog.csdn.net/Roland_Sun/article/details/50266565	（了解整额技术的全局）
- http://pubs.opengroup.org/onlinepubs/7908799/xns/connect.html （查看指定API）

这次学习到的技术点为Linux下的IPC的一种方式：unix domain socket，是对于本机不同进程相互通信的一种套接字，但没有经过协议栈因此效率会比网络传输的更快、更稳定也是经常使用的一种IPC方式；

首先也是根据参考资料来确定使用那些API以及使用API的流程，每个文章一般介绍的方式都不同所以不好统一，我这里使用的结构是

	+---------------+                                 +---------------+
	|               | recvfrom                sendto  |               |
	|               |                                 |               |
	|     read      | <------- /tmp/socket <--------  |     write     |
	|               |                                 |               |
	|               |                                 |               |
	+---------------+                                 +---------------+

具体的代码详见该文章所在目录的read.c、write.c

#### 踩到的一些坑： ####

1. 写完demo后一直server端一直无法显示接到的请求，后来经朋友检查发现是由于printf没有添加"\n"导致所有发过来的信息都存在缓冲区中没有刷新到屏幕上。原因是因为自己对printf理解不深导致，卡了近一天的时间.

	解决方法：使用fflush在每次接收完毕后使用；直接在输出中添加\n换行符；或者直接输入超过1024字节的(1024为缓冲区大小，了解为主)

2. 一开始知道这个技术的时候并没有很顺利的实现出来，原因是因为不了解整个技术的过程以及整个过程中需要使用那些API，这些东西当时都卡了很久，后来也是通过google一个一个解决，先跑起来为主

#### 总结： ####

- 对于debug不能完全相信printf可能因为对printf的不了解导致自己对程序的执行判断出现错误；
- 对于socket中sendto和recvfrom为一对；send和recv为一对，send发送的套接字必须是connect()过的
- 其实一开始也是很迷茫到底要分享什么，后来决定去开源项目中找点自己不会的来实现就有了这篇文章，我看的项目是开源的`HIDS OSSEC`,其中有个模块为`remoted`是用来接收`agent`发送过来的信息并转发到本地的`analysed`进程这中间就使用了`unix domain socket`技术用来传递信息.


## Share ##

这次分享主要谈谈自己对于ARTS每个方面的一些思考：

对于Algorithm的练习，主要是为了锻炼自己的解题思路，同时在做题时，最好先自己头脑分析，整理好方案后，一次代码编译出结果。切勿边想边做，这样会影响效率。同时集中注意力避免一些低级的编码bug。最后分享时主要以思路分享为主。

对于Review这一块一开始自己很困惑不知道要怎么去做。后来想明白这一块主要是为了加深对一些知识的理解，夯实基础。 通过对自己以前掌握的一些技术点进行重塑以及加深理解。

对于Technique，主要是学习之前未掌握的技术点，通过理论学习再配合上实践达到可以掌握使用方法，应用到具体的场景中。

对于Share，主要分享自己对于某些东西的感悟认识和感触(都是通过实践得来的)

以上为ARTS的一些感触，如果实在不知道如何准备可以参考一下:P

