#pragma once
#include"TextObject.h"

#include"TextStyle.h"

//************************************************************************************
//用于和用户间传输数据的结构体，包含字符串和相应的修饰信息
struct SegStruct {
	int segIndex;	//段标号
	char segState;	//段状态，包括初始、新建、删除、修改和取消五个状态。撤销操作时用户可根据返回段结点的状态做相应处理
	std::string segStr;	//段的字符串
	std::list<TypeNode> typeList;	//与字符串对应的格式链表。每个段结点包含一个不定长的格式链表保存段的修饰信息，每次修改段的字体型号、颜色等都可以通过向链表后加入新结点完成
};


//************************************************************************************
//RichSegNode（富段结点）主要包含段的字符串和关联的格式链表TypeList等属性以及相关操作
class RichSegNode {

public:
	//构造函数
	RichSegNode() :
		m_segLogIndex(-1),
		m_age(0),
		m_segState(SEG_INITAIL),
		m_segStr(""),
		m_pNextSegTextObject(nullptr),
		m_pPreSegTextObject(nullptr)
	{
		//增加全局格式结点,宋体黑色四号字体，同时给m_TypeList赋值
	};

	//析构函数
	~RichSegNode() {};

	//获取格式链表头指针
	std::list<TypeNode> getTypeList()const;

	//设置字符串
	bool setSegStr(const std::string &str);
	//获取字符串
	std::string getSegStr()const;

	//设置segLogIndex
	bool setSegLogIndex(int segIndex);
	//获取segLogIndex
	int getSegLogIndex()const;

	//设置segPhyIndex
	bool setSegPhyIndex(int segIndex);
	//获取segPhyIndex
	int getSegPhyIndex()const;

	//设置age
	bool setAge(int tAge);
	//获取age
	int getAge();

	//设置segState
	bool setSegState(char segState);
	//获取segState
	char getSegState();

	//设置pNextSegTextObject
	bool setNextSegTextObject(std::shared_ptr<RichSegNode> pRichTextNode);
	//获取pNextSegTextObject
	std::shared_ptr<RichSegNode> getNextSegTextObject()const;

	//设置pNextSegTextObject
	bool setPreSegTextObject(std::shared_ptr<RichSegNode> pRichTextNode);
	//获取pNextSegTextObject
	std::shared_ptr<RichSegNode> getPreSegTextObject()const;

	////增加格式结点,加入到TypeList链表尾
	//bool addTypeNode(TypeRange theTypeRange, TEXTTYPE textType);

private:


private:
	int m_segLogIndex;	//逻辑段号是对应用户提交请求中的段号
	int m_segPhyIndex;	//物理段号是段结点回到到LineSpace的依据，对在LineSpace中取出的段，物理段号对应其在LineSpace中段号，否则和逻辑段号相同
	int m_age;	//用于记录结点在历史链表中的老化程度，数制越大越老，达到最大值从ModList中剔除
	char m_segState;	//段结点的状态,包括初始态、新建态、删除态、修改态、取消态，在ModList中处理的关键信息，也是用户执行撤销操作的依据
	std::string m_segStr;	//段内字符串
	std::list<TypeNode> m_TypeList;	//修饰链表
	std::shared_ptr<RichSegNode> m_pNextSegTextObject;	//指向下个段结点
	std::shared_ptr<RichSegNode> m_pPreSegTextObject;	//指向上个段结点
};



//************************************************************************************
//控制端,负责M端和V端数据的交换，并且维护着两个队列，分别是预存取队列（PreList）和历史修改队列（ModList）
class TextObjectCtrol {

public:
	//构造函数
	TextObjectCtrol(std::shared_ptr<TextObject> pTextObject):	//创建TextObjectCtrol对象需要传入指向TextObject的指针
		m_pHandleSegNode(nullptr),
		m_modListCount(0),
		m_pHeadOfModList(nullptr),
		m_pEndOfModList(nullptr),
		m_pBackSegIndex(nullptr)
	{
		if (m_pTextObject = pTextObject)//设置m_pTextObject，这是TextObjectCtrol正常工作的前提
			m_segLogCount = m_pTextObject->getSegmentCount();	//m_pTextObject不为nullptr时设置段数初值
	};

	//析构函数
	~TextObjectCtrol() {};


	//用户获取指定段内容，需要指定段号，segStr和typeList在函数结束后得到
	bool userGetSegment(SegStruct &segStruct);
	//用户新增段落，传入的segStruct要指定segStr和typeList
	bool userAddSegment(const SegStruct &segStruct);
	//用户修改段落，传入的segStruct要指定segStr和typeList
	bool userReviseSegment(const SegStruct &segStruct);
	//用户删除指定段落，传入的segStruct只要指定段号
	bool userDeleteSegment(int segLogIndex);
	//用户撤销操作,即获取处理段。传入的segStruct无需指定任何值，函数结束后得到所有项的值segIndex、segStr、typeList和segState
	bool userBackSegment(SegStruct &segStruct);
	//获取逻辑段数
	int getSegLogCount()const;



private:
	//在链表中查找segIndex指定段结点，找到返回指向对应结点指针
	bool searchRichSegNode(int segLogIndex, int &segPhyIndex, std::string &segStr, std::list<TypeNode> &typeList);
	//获取物理段号，即段在内存中实际的段号
	int getPhyIndex(int segLogIndex);
	//设置当前处理段结点，同时将之前的段结点转移到ModList中
	bool setHandleSegNode(int segIndex, int segPhyIndex, const std::string &segStr,const std::list<TypeNode> &typeList, char segState = SEG_INITAIL);
	//将指定结点转移到历史队列ModList.
	bool moveToModList(std::shared_ptr<RichSegNode> pNewNode);
	//探查历史修改队列，对老化程度检测，到达指定条件的结点回写到TextObject中
	int backWriteQuery();


private:
	int m_segLogCount;	//当前存在段数量
	std::shared_ptr<TextObject> m_pTextObject;	//指向TextObject指针，用于访问LineSpace
	std::shared_ptr<RichSegNode> m_pHandleSegNode;	//指向当前处理段结点
	int m_modListCount;	//ModList结点数
	std::shared_ptr<RichSegNode> m_pHeadOfModList;	//指向保存历史修改链表的表首
	std::shared_ptr<RichSegNode> m_pEndOfModList;	//指向保存历史修改链表的表尾
	std::shared_ptr<RichSegNode> m_pBackSegIndex;	//可撤销结点指针
};