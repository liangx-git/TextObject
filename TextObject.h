#ifndef ONE_H
#define ONE_H
#pragma  warning( disable: 4996 )

#include <Afxwin.h>	//使用string
#include<iostream>	//支持string
#include <fstream>  //支持fstream
#include<string>
#include<memory>	//使用智能指针

#define MAX_CHARS 50			//每个块链保存字符数
#define MAX_LINES 1000			//块链数组最大行数
#define MAX_SEGMENTS 50			//段数组容量

#include"TextStyle.h"

//*********************************************************************
//块链节点类
class LineElement {

public:
	//构造函数
	LineElement() :llink(-1), rlink(-1) {};
	//析构函数
	~LineElement() {};

public:
	int llink;	//左结点下标
	int rlink;	//右结点下标
	char charArray[MAX_CHARS];	//字符数组
};

typedef class LineElement *PLineElement, *LineSpace;




//***********************************************************************
//LineSpaceManage类负责管理内存，即管理LineSpace，所有段对象共享一个
//
//***********************************************************************

class LineSpaceManage {

public:
	//构造函数
	LineSpaceManage() :
		m_usedLinesCount(0),
		m_freeLineCount(MAX_LINES),
		m_freeHeadIndex(0)
	{
		initLineSpace();
	};
	//析构函数
	~LineSpaceManage() {};

	//读取行内容
	bool getLinesContent(int headIndex, std::string &str)const;
	//写入段内容
	bool setLinesContent(int &headIndex, const std::string &str);
	//是否还有可用块链
	int hasEmptyLine()const;
	//回收段中所有的行
	bool retrieveLines(int &headIndex);

private:
	//创建初始化的LineSpace
	bool initLineSpace();
	//回收块链结点到FreeSpace，只在当前函数delete字符数组
	bool retrieveFreeLine(int retrieveIndex);
	//从自由段中申请空块链结点设置为charArray，如果申请成功则返回新块链的标号，否则返回-1
	int applyFreeLine();

private:
	LineElement m_lineSpace[MAX_LINES];	//LineSpace数组
	int m_usedLinesCount;	//当前块链数组被使用行数
	int m_freeLineCount;	//当前可用的链块数
	int m_freeHeadIndex;	//free链表头的下标
};
typedef class LineSpaceManage *PLineSpaceManage;




//***********************************************************************
// SegElement类保存段的信息，包括m_segIndex指向存放在在LineSpace中字符串
//的首元块链下标，各个段对象的m_lineSpaceManage指向同一个LineSpaceManage；
//***********************************************************************

typedef class SegElement  *PSegElement;
//段元素类
class SegElement {

public:
	//构造函数
	//SegElement() :m_segIndex(-1), m_pNextSegment(0) {};
	//带参数构造函数
	SegElement(std::shared_ptr<LineSpaceManage> pLineSpaceManage) :
		m_segIndex(-1),
		m_pNextSegment(nullptr)
	{
		m_pLineSpaceManage = pLineSpaceManage;
	};

	//析构函数
	~SegElement() {};

	//获取段内容
	bool getSegContent(std::string &str)const;
	//设置段内容
	bool setSegContent(const std::string &str);
	//获取段格式链表
	bool getSegTypeList(std::list<TypeNode> &typeList);
	//设置段的格式链表
	bool setSegTypeList(std::list<TypeNode> &typeList);
	//删除段内容
	bool deleteSegContent();
	//设置下个段结点指针
	bool setNextSegment(std::shared_ptr<SegElement> pSeg);
	//获取下个段结点指针
	std::shared_ptr<SegElement> getNextSegment();

private:
	int m_segIndex;	//段首行下标，相当于到LIneSpace中获取字符串的钥匙
	std::shared_ptr<LineSpaceManage> m_pLineSpaceManage;	//所有段对象共享一个LineSpaceManage
	std::list<TypeNode> m_TypeList;	//格式链表
	std::shared_ptr<SegElement> m_pNextSegment;	//指向下个段
};



//***********************************************************************
//	TextObject类是面向用户最外层的类。包含两个成员变量，m_pHeadOfSegArray
//指向段对象数组的首元地址，m_segCount记录段数目。
//***********************************************************************
class TextObject {
public:
	//构造函数
	TextObject() :
		m_segCount(0),
		m_pHeadOfSegArray(nullptr),
		//m_pPenultSegArray(nullptr),
		m_pEndOfSegArray(nullptr)
	{
		//创建TextObject同时创建LineSpaceManage,随后创建的段对象都获得指向该LineSpaceManage的指针
		m_mLineSpaceManage = std::make_shared<LineSpaceManage>();
		readText();
	};

	~TextObject() {};

	//增加新段,segIndex=-1表示添加到第一段（下标从0开始）
	bool insertNewSegmentPost(int segIndex, const std::string &str, std::list<TypeNode> &typeList);
	//删除段
	bool deleteSegment(int segIndex);
	//修改段
	bool changeSegment(int segIndex, const std::string &str, std::list<TypeNode> &typeList);
	//获取段内容
	bool getSegmentContent(int segIndex, std::string &str,std::list<TypeNode> &typeList);
	//获取段数
	int getSegmentCount();
	//读取文本
	bool readText(/*CFile *cFile*/);
private:
	//查找segIndex指定段，若找到返回指向该段的指针 
	std::shared_ptr<SegElement> searchSegment(int segIndex)const;

private:
	int m_segCount;	//记录当前段对象数量
	std::shared_ptr<SegElement> m_pHeadOfSegArray;	//指向段链表的头
	//std::shared_ptr<SegElement> m_pPenultSegArray;	//指向段表表尾结点的上个结点(倒数第二)，便于删除
	std::shared_ptr<SegElement> m_pEndOfSegArray;	//指向段表表尾结点，便于添加
	std::shared_ptr<LineSpaceManage> m_mLineSpaceManage;
};

#endif
