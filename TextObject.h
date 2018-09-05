#ifndef ONE_H
#define ONE_H
#pragma  warning( disable: 4996 )

#include <Afxwin.h>	//ʹ��string
#include<iostream>	//֧��string
#include <fstream>  //֧��fstream
#include<string>
#include<memory>	//ʹ������ָ��

#define MAX_CHARS 50			//ÿ�����������ַ���
#define MAX_LINES 1000			//���������������
#define MAX_SEGMENTS 50			//����������

#include"TextStyle.h"

//*********************************************************************
//�����ڵ���
class LineElement {

public:
	//���캯��
	LineElement() :llink(-1), rlink(-1) {};
	//��������
	~LineElement() {};

public:
	int llink;	//�����±�
	int rlink;	//�ҽ���±�
	char charArray[MAX_CHARS];	//�ַ�����
};

typedef class LineElement *PLineElement, *LineSpace;




//***********************************************************************
//LineSpaceManage�ฺ������ڴ棬������LineSpace�����жζ�����һ��
//
//***********************************************************************

class LineSpaceManage {

public:
	//���캯��
	LineSpaceManage() :
		m_usedLinesCount(0),
		m_freeLineCount(MAX_LINES),
		m_freeHeadIndex(0)
	{
		initLineSpace();
	};
	//��������
	~LineSpaceManage() {};

	//��ȡ������
	bool getLinesContent(int headIndex, std::string &str)const;
	//д�������
	bool setLinesContent(int &headIndex, const std::string &str);
	//�Ƿ��п��ÿ���
	int hasEmptyLine()const;
	//���ն������е���
	bool retrieveLines(int &headIndex);

private:
	//������ʼ����LineSpace
	bool initLineSpace();
	//���տ�����㵽FreeSpace��ֻ�ڵ�ǰ����delete�ַ�����
	bool retrieveFreeLine(int retrieveIndex);
	//�����ɶ�������տ����������ΪcharArray���������ɹ��򷵻��¿����ı�ţ����򷵻�-1
	int applyFreeLine();

private:
	LineElement m_lineSpace[MAX_LINES];	//LineSpace����
	int m_usedLinesCount;	//��ǰ�������鱻ʹ������
	int m_freeLineCount;	//��ǰ���õ�������
	int m_freeHeadIndex;	//free����ͷ���±�
};
typedef class LineSpaceManage *PLineSpaceManage;




//***********************************************************************
// SegElement�ౣ��ε���Ϣ������m_segIndexָ��������LineSpace���ַ���
//����Ԫ�����±꣬�����ζ����m_lineSpaceManageָ��ͬһ��LineSpaceManage��
//***********************************************************************

typedef class SegElement  *PSegElement;
//��Ԫ����
class SegElement {

public:
	//���캯��
	//SegElement() :m_segIndex(-1), m_pNextSegment(0) {};
	//���������캯��
	SegElement(std::shared_ptr<LineSpaceManage> pLineSpaceManage) :
		m_segIndex(-1),
		m_pNextSegment(nullptr)
	{
		m_pLineSpaceManage = pLineSpaceManage;
	};

	//��������
	~SegElement() {};

	//��ȡ������
	bool getSegContent(std::string &str)const;
	//���ö�����
	bool setSegContent(const std::string &str);
	//��ȡ�θ�ʽ����
	bool getSegTypeList(std::list<TypeNode> &typeList);
	//���öεĸ�ʽ����
	bool setSegTypeList(std::list<TypeNode> &typeList);
	//ɾ��������
	bool deleteSegContent();
	//�����¸��ν��ָ��
	bool setNextSegment(std::shared_ptr<SegElement> pSeg);
	//��ȡ�¸��ν��ָ��
	std::shared_ptr<SegElement> getNextSegment();

private:
	int m_segIndex;	//�������±꣬�൱�ڵ�LIneSpace�л�ȡ�ַ�����Կ��
	std::shared_ptr<LineSpaceManage> m_pLineSpaceManage;	//���жζ�����һ��LineSpaceManage
	std::list<TypeNode> m_TypeList;	//��ʽ����
	std::shared_ptr<SegElement> m_pNextSegment;	//ָ���¸���
};



//***********************************************************************
//	TextObject���������û��������ࡣ����������Ա������m_pHeadOfSegArray
//ָ��ζ����������Ԫ��ַ��m_segCount��¼����Ŀ��
//***********************************************************************
class TextObject {
public:
	//���캯��
	TextObject() :
		m_segCount(0),
		m_pHeadOfSegArray(nullptr),
		//m_pPenultSegArray(nullptr),
		m_pEndOfSegArray(nullptr)
	{
		//����TextObjectͬʱ����LineSpaceManage,��󴴽��Ķζ��󶼻��ָ���LineSpaceManage��ָ��
		m_mLineSpaceManage = std::make_shared<LineSpaceManage>();
		readText();
	};

	~TextObject() {};

	//�����¶�,segIndex=-1��ʾ��ӵ���һ�Σ��±��0��ʼ��
	bool insertNewSegmentPost(int segIndex, const std::string &str, std::list<TypeNode> &typeList);
	//ɾ����
	bool deleteSegment(int segIndex);
	//�޸Ķ�
	bool changeSegment(int segIndex, const std::string &str, std::list<TypeNode> &typeList);
	//��ȡ������
	bool getSegmentContent(int segIndex, std::string &str,std::list<TypeNode> &typeList);
	//��ȡ����
	int getSegmentCount();
	//��ȡ�ı�
	bool readText(/*CFile *cFile*/);
private:
	//����segIndexָ���Σ����ҵ�����ָ��öε�ָ�� 
	std::shared_ptr<SegElement> searchSegment(int segIndex)const;

private:
	int m_segCount;	//��¼��ǰ�ζ�������
	std::shared_ptr<SegElement> m_pHeadOfSegArray;	//ָ��������ͷ
	//std::shared_ptr<SegElement> m_pPenultSegArray;	//ָ��α��β�����ϸ����(�����ڶ�)������ɾ��
	std::shared_ptr<SegElement> m_pEndOfSegArray;	//ָ��α��β��㣬�������
	std::shared_ptr<LineSpaceManage> m_mLineSpaceManage;
};

#endif
