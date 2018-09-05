#pragma once

#include<List>	//使用链表


#define TEXTTYPE_NULL 0x00000000	//无修饰
#define TEXTTYPE_EX_ITALIC 0x00000001	//加粗
#define TEXTTYPE_EX_UNDERLINE 0x00000002	//下划线
#define TEXTTYPE_EX_STRIKEOUT 0x00000004	//删除线

#define TEXTTYPE_COLOR_RED	0x00000100	//红色
#define TEXTTYPE_COLOR_BLACK 0x00000200	//黑色

#define TEXTTYPE_FONT_SONTI	0x00010000	//宋体

#define TEXTTYPE_SIZE_FOUR 0x01000000	//四号字体
#define TEXTTYPE_
#define TEXTTYPE_

#define SEG_INITAIL	0x00	//初始态，
#define SEG_NEW 0x01	//新建态
#define SEG_DELETED 0x02	//删除态
#define SEG_MODIFIED 0x04	//修改态
#define SEG_CANCLE	0x08	//取消态


#define MAXPRECOUNT 3	//预取队列容量
#define MAXBACKUPCOUNT 5	//历史修改队列最大节点数，也是撤销操作的最大次数
#define MAXAGE 5	//最大老化值

typedef  unsigned long TEXTTYPE;	//自定义字体属性类型为四个字节无符号整数，后期可换成八个字节



//************************************************************************************
//TypeNode中确定修饰范围的结构，beg和end确定修饰范围大小，值为-1表示从字符串头到尾
struct TypeRange {
	TypeRange(int beg = -1, int end = -1) :beg(beg), end(end) {};	//构造函数1
	TypeRange(const TypeRange &typeRange) :beg(typeRange.beg), end(typeRange.end) {};	//构造函数2
	int beg;	//开始字符标号
	int end;	//结束字符下标
};


//************************************************************************************
//格式（修饰）链表结点
struct TypeNode {
	//构造函数
	TypeNode(TEXTTYPE textType = TEXTTYPE_NULL, TypeRange typeRange = TypeRange(-1, -1)) :
		textType(textType),
		typeRange(typeRange) {};

	TEXTTYPE textType;	//修饰类型，4字节无符号整数,当位向量使用
	TypeRange typeRange;	//修饰范围包括首位位置坐标
};

