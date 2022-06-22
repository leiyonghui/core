#pragma once

/*
(1)#error msg 指令使編譯器停止執行並打印一條語句，
(2)printf("%d,%s", _LINE_, _FILE_)打印當前行號和文件名
(3)#pragma arg 設置了編譯器所使用的條件.
(4)#str 將字符串放入被引號括起來的字符串中，如

尽可能的少用公用宏,能替换掉就替换掉；
对于泛型预处理程序，我们可以用函数模板或类默板来代替：
符号常量预处理程序我们可以用 const or enum 来代替：
非泛型内联函数的预处理程序可以使用真正的内联函数来代替：
*/

#define FPOS(type, field) ((uint64)&((type*)0)->field)	//得到一个 field 在结构体(struct)中的偏移量
#define FSIZ(type, field) sizeof(((type *)0)->field)	//得到一个结构体中 field 所占用的字节数
#define ARR_SIZE(a) (sizeof((a))/sizeof((a[0])))		//返回数组元素的个数

//保证总是使用分号正确写法
#define WARN_IF(EXP) \
	do { if (EXP) \
		fprintf(stderr, "Warning: " #EXP "\n"); \
	}while (0)

/*Duplication of Side Effects
这里的 Side Effect 是指宏在展开的时候对其参数可能进行多次 Evaluation（也就是取值），但是如果
这个宏参数是一个函数，那么就有可能被调用多次从而达到不一致的结果，甚至会发生更严重的错误。*/
#define core_find(container, key, value) (container.find(key) == container.end() ? value : container.find(key)->second)
#define MIN(X,Y) ({ \
typeof(X) x_ = (X); \
typeof(Y) y_ = (Y); \
(x_ < y_) ? x_ : y_; })

//#define USING_SHARE_PTR(Name) typedef std::shared_ptr<Name> Name##Ptr
#define USING_SHARED_PTR(Name) using Name##Ptr = std::shared_ptr<Name>

#define DYNAMIC_CAST(CLASS, VALUE) std::dynamic_pointer_cast<CALSS>(VALUE)
#define STATIC_CAST(CLASS, VALUE) std::static_pointer_cast<CLASS>(CALUE)

#define SHARED_THIS(CLASS) std::static_pointer_cast<CLASS>(shared_from_this())