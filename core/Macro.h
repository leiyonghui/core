#pragma once

/*
(1)#error msg ָ��ʹ���g��ֹͣ���ЁK��ӡһ�l�Z�䣬
(2)printf("%d,%s", _LINE_, _FILE_)��ӡ��ǰ��̖���ļ���
(3)#pragma arg �O���˾��g����ʹ�õėl��.
(4)#str ���ַ������뱻��̖��������ַ����У���

�����ܵ����ù��ú�,���滻�����滻����
���ڷ���Ԥ����������ǿ����ú���ģ�����Ĭ�������棺
���ų���Ԥ����������ǿ����� const or enum �����棺
�Ƿ�������������Ԥ����������ʹ���������������������棺
*/

#define FPOS(type, field) ((uint64)&((type*)0)->field)	//�õ�һ�� field �ڽṹ��(struct)�е�ƫ����
#define FSIZ(type, field) sizeof(((type *)0)->field)	//�õ�һ���ṹ���� field ��ռ�õ��ֽ���
#define ARR_SIZE(a) (sizeof((a))/sizeof((a[0])))		//��������Ԫ�صĸ���

//��֤����ʹ�÷ֺ���ȷд��
#define WARN_IF(EXP) \
	do { if (EXP) \
		fprintf(stderr, "Warning: " #EXP "\n"); \
	}while (0)

/*Duplication of Side Effects
����� Side Effect ��ָ����չ����ʱ�����������ܽ��ж�� Evaluation��Ҳ����ȡֵ�����������
����������һ����������ô���п��ܱ����ö�δӶ��ﵽ��һ�µĽ���������ᷢ�������صĴ���*/
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