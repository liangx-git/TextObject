#pragma once
#include"TextObject.h"

#include"TextStyle.h"

//************************************************************************************
//���ں��û��䴫�����ݵĽṹ�壬�����ַ�������Ӧ��������Ϣ
struct SegStruct {
	int segIndex;	//�α��
	char segState;	//��״̬��������ʼ���½���ɾ�����޸ĺ�ȡ�����״̬����������ʱ�û��ɸ��ݷ��ضν���״̬����Ӧ����
	std::string segStr;	//�ε��ַ���
	std::list<TypeNode> typeList;	//���ַ�����Ӧ�ĸ�ʽ����ÿ���ν�����һ���������ĸ�ʽ������ε�������Ϣ��ÿ���޸Ķε������ͺš���ɫ�ȶ�����ͨ�������������½�����
};


//************************************************************************************
//RichSegNode�����ν�㣩��Ҫ�����ε��ַ����͹����ĸ�ʽ����TypeList�������Լ���ز���
class RichSegNode {

public:
	//���캯��
	RichSegNode() :
		m_segLogIndex(-1),
		m_age(0),
		m_segState(SEG_INITAIL),
		m_segStr(""),
		m_pNextSegTextObject(nullptr),
		m_pPreSegTextObject(nullptr)
	{
		//����ȫ�ָ�ʽ���,�����ɫ�ĺ����壬ͬʱ��m_TypeList��ֵ
	};

	//��������
	~RichSegNode() {};

	//��ȡ��ʽ����ͷָ��
	std::list<TypeNode> getTypeList()const;

	//�����ַ���
	bool setSegStr(const std::string &str);
	//��ȡ�ַ���
	std::string getSegStr()const;

	//����segLogIndex
	bool setSegLogIndex(int segIndex);
	//��ȡsegLogIndex
	int getSegLogIndex()const;

	//����segPhyIndex
	bool setSegPhyIndex(int segIndex);
	//��ȡsegPhyIndex
	int getSegPhyIndex()const;

	//����age
	bool setAge(int tAge);
	//��ȡage
	int getAge();

	//����segState
	bool setSegState(char segState);
	//��ȡsegState
	char getSegState();

	//����pNextSegTextObject
	bool setNextSegTextObject(std::shared_ptr<RichSegNode> pRichTextNode);
	//��ȡpNextSegTextObject
	std::shared_ptr<RichSegNode> getNextSegTextObject()const;

	//����pNextSegTextObject
	bool setPreSegTextObject(std::shared_ptr<RichSegNode> pRichTextNode);
	//��ȡpNextSegTextObject
	std::shared_ptr<RichSegNode> getPreSegTextObject()const;

	////���Ӹ�ʽ���,���뵽TypeList����β
	//bool addTypeNode(TypeRange theTypeRange, TEXTTYPE textType);

private:


private:
	int m_segLogIndex;	//�߼��κ��Ƕ�Ӧ�û��ύ�����еĶκ�
	int m_segPhyIndex;	//����κ��Ƕν��ص���LineSpace�����ݣ�����LineSpace��ȡ���ĶΣ�����κŶ�Ӧ����LineSpace�жκţ�������߼��κ���ͬ
	int m_age;	//���ڼ�¼�������ʷ�����е��ϻ��̶ȣ�����Խ��Խ�ϣ��ﵽ���ֵ��ModList���޳�
	char m_segState;	//�ν���״̬,������ʼ̬���½�̬��ɾ��̬���޸�̬��ȡ��̬����ModList�д���Ĺؼ���Ϣ��Ҳ���û�ִ�г�������������
	std::string m_segStr;	//�����ַ���
	std::list<TypeNode> m_TypeList;	//��������
	std::shared_ptr<RichSegNode> m_pNextSegTextObject;	//ָ���¸��ν��
	std::shared_ptr<RichSegNode> m_pPreSegTextObject;	//ָ���ϸ��ν��
};



//************************************************************************************
//���ƶ�,����M�˺�V�����ݵĽ���������ά�����������У��ֱ���Ԥ��ȡ���У�PreList������ʷ�޸Ķ��У�ModList��
class TextObjectCtrol {

public:
	//���캯��
	TextObjectCtrol(std::shared_ptr<TextObject> pTextObject):	//����TextObjectCtrol������Ҫ����ָ��TextObject��ָ��
		m_pHandleSegNode(nullptr),
		m_modListCount(0),
		m_pHeadOfModList(nullptr),
		m_pEndOfModList(nullptr),
		m_pBackSegIndex(nullptr)
	{
		if (m_pTextObject = pTextObject)//����m_pTextObject������TextObjectCtrol����������ǰ��
			m_segLogCount = m_pTextObject->getSegmentCount();	//m_pTextObject��Ϊnullptrʱ���ö�����ֵ
	};

	//��������
	~TextObjectCtrol() {};


	//�û���ȡָ�������ݣ���Ҫָ���κţ�segStr��typeList�ں���������õ�
	bool userGetSegment(SegStruct &segStruct);
	//�û��������䣬�����segStructҪָ��segStr��typeList
	bool userAddSegment(const SegStruct &segStruct);
	//�û��޸Ķ��䣬�����segStructҪָ��segStr��typeList
	bool userReviseSegment(const SegStruct &segStruct);
	//�û�ɾ��ָ�����䣬�����segStructֻҪָ���κ�
	bool userDeleteSegment(int segLogIndex);
	//�û���������,����ȡ����Ρ������segStruct����ָ���κ�ֵ������������õ��������ֵsegIndex��segStr��typeList��segState
	bool userBackSegment(SegStruct &segStruct);
	//��ȡ�߼�����
	int getSegLogCount()const;



private:
	//�������в���segIndexָ���ν�㣬�ҵ�����ָ���Ӧ���ָ��
	bool searchRichSegNode(int segLogIndex, int &segPhyIndex, std::string &segStr, std::list<TypeNode> &typeList);
	//��ȡ����κţ��������ڴ���ʵ�ʵĶκ�
	int getPhyIndex(int segLogIndex);
	//���õ�ǰ����ν�㣬ͬʱ��֮ǰ�Ķν��ת�Ƶ�ModList��
	bool setHandleSegNode(int segIndex, int segPhyIndex, const std::string &segStr,const std::list<TypeNode> &typeList, char segState = SEG_INITAIL);
	//��ָ�����ת�Ƶ���ʷ����ModList.
	bool moveToModList(std::shared_ptr<RichSegNode> pNewNode);
	//̽����ʷ�޸Ķ��У����ϻ��̶ȼ�⣬����ָ�������Ľ���д��TextObject��
	int backWriteQuery();


private:
	int m_segLogCount;	//��ǰ���ڶ�����
	std::shared_ptr<TextObject> m_pTextObject;	//ָ��TextObjectָ�룬���ڷ���LineSpace
	std::shared_ptr<RichSegNode> m_pHandleSegNode;	//ָ��ǰ����ν��
	int m_modListCount;	//ModList�����
	std::shared_ptr<RichSegNode> m_pHeadOfModList;	//ָ�򱣴���ʷ�޸�����ı���
	std::shared_ptr<RichSegNode> m_pEndOfModList;	//ָ�򱣴���ʷ�޸�����ı�β
	std::shared_ptr<RichSegNode> m_pBackSegIndex;	//�ɳ������ָ��
};