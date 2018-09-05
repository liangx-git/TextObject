//#include"stdafx.h"
#include "FrontTextObject.h"


//***************************************RichSegNode***************************************************************
bool RichSegNode::setSegStr(const std::string &str)
{
	if ((m_segStr = str) != "")
		return true;
	else
		return false;
}

std::string RichSegNode::getSegStr()const
{
	return m_segStr;
}

std::list<TypeNode> RichSegNode::getTypeList() const
{
	return m_TypeList;
}

bool RichSegNode::setSegLogIndex(int segIndex)
{
	m_segLogIndex = segIndex;
	return true;
}

int RichSegNode::getSegLogIndex() const
{
	return m_segLogIndex;
}

bool RichSegNode::setSegPhyIndex(int segIndex)
{
	m_segPhyIndex = segIndex;
	return true;
}

int RichSegNode::getSegPhyIndex() const
{
	return m_segPhyIndex;
}

bool RichSegNode::setAge(int tAge)
{
	m_age = tAge;
	return true;
}

int RichSegNode::getAge()
{
	return m_age;
}

bool RichSegNode::setSegState(char segState)
{
	m_segState = segState;
	return true;
}

char RichSegNode::getSegState()
{
	return m_segState;
}

bool RichSegNode::setNextSegTextObject(std::shared_ptr<RichSegNode> pRichTextNode)
{
	if (m_pNextSegTextObject = pRichTextNode)
		return true;
	else
		return false;
}

std::shared_ptr<RichSegNode> RichSegNode::getNextSegTextObject() const
{
	return m_pNextSegTextObject;
}

bool RichSegNode::setPreSegTextObject(std::shared_ptr<RichSegNode> pRichTextNode)
{
	m_pPreSegTextObject = pRichTextNode;
	return true;
}

std::shared_ptr<RichSegNode> RichSegNode::getPreSegTextObject() const
{
	return m_pPreSegTextObject;
}


//*****************************************TextObjectCtrol*************************************************************

bool TextObjectCtrol::userGetSegment(SegStruct &segStruct)
{
	int segPhyIndex;
	std::string segStr;
	std::list<TypeNode> typeList;
	if (searchRichSegNode(segStruct.segIndex, segPhyIndex, segStr, typeList)) {	//���Ҷν��
		if (setHandleSegNode(segStruct.segIndex, segPhyIndex, segStr, typeList)) {	//���µ�ǰ������
			segStruct.segStr = segStr;	//���ض����ַ���
			segStruct.typeList = typeList;	//����TypeListͷָ��
			return true;
		}
	}
	return false;
}

bool TextObjectCtrol::userAddSegment(const SegStruct & segStruct)
{
	if (!setHandleSegNode(segStruct.segIndex, segStruct.segIndex, segStruct.segStr, segStruct.typeList,SEG_NEW))	//�½��ν��
		return false;								
	return true;
}

bool TextObjectCtrol::userReviseSegment(const SegStruct &segStruct)
{
	int segPhyIndex;
	std::string segStr;
	std::list<TypeNode> typeList;
	if (searchRichSegNode(segStruct.segIndex, segPhyIndex, segStr, typeList)) {	//���Ҷν���ȡ����κ�
		if (setHandleSegNode(segStruct.segIndex, segPhyIndex, segStruct.segStr, segStruct.typeList,SEG_MODIFIED)) 	//���µ�ǰ������
			return true;
	}
	return false;
}

bool TextObjectCtrol::userDeleteSegment(int segLogIndex)
{
	int segPhyIndex;
	std::string segStr;
	std::list<TypeNode> typeList;
	if (searchRichSegNode(segLogIndex, segPhyIndex, segStr, typeList)) {	//����Ҫɾ���Ľ���Ƿ����
		if (!setHandleSegNode(segLogIndex, segPhyIndex, segStr, typeList, SEG_DELETED))
			return false;
		return true;
	}
	return false;
}

bool TextObjectCtrol::userBackSegment(SegStruct & segStruct)
{
	if (m_pBackSegIndex) {	//�ɳ������ָ�벻Ϊ����
		while (m_pBackSegIndex&&m_pBackSegIndex->getSegState()&SEG_CANCLE) 	//��ȡ��ΪSEG_CANCLE̬���
			m_pBackSegIndex = m_pBackSegIndex->getNextSegTextObject();	//�ó���ָ��ָ���¸����
		if (m_pBackSegIndex==nullptr)	//ָ��Ϊ��û�пɳ����Ľ��
			return false;
		if (m_pBackSegIndex->getSegState() == (SEG_DELETED | SEG_CANCLE)) {	//��������ɾ�������е��������
			std::shared_ptr<RichSegNode> pTemp = m_pBackSegIndex;
			while (pTemp) {
				if ((pTemp->getSegLogIndex() == m_pBackSegIndex->getSegLogIndex()) && (pTemp->getSegState() == (SEG_CANCLE | SEG_NEW))) 	//��ȡ֮ǰ��ȡ���������½�̬���ָ��
					pTemp->setSegState(pTemp->getSegState()&~SEG_CANCLE);	//�ָ�����̬����״̬
				pTemp = pTemp->getNextSegTextObject();	//��ȡ�¸����ָ��
			}
		}
		else {
			m_pBackSegIndex->setSegState(m_pBackSegIndex->getSegState() | SEG_CANCLE);	//һ��Ľ��ֻҪ����״̬��Ϊ����SEG_CANCLE����
		}

		m_pHandleSegNode->setSegState(m_pHandleSegNode->getSegState() | SEG_CANCLE);	//������ҲҪ��HandleSegNodeΪ��Ч̬
		//��ȡ�����������Ľ������
		segStruct.segIndex = m_pBackSegIndex->getSegLogIndex();
		segStruct.segStr = m_pBackSegIndex->getSegStr();
		segStruct.typeList = m_pBackSegIndex->getTypeList();
		return true;
	}
	return false;
	
}

int TextObjectCtrol::getSegLogCount() const
{
	return m_segLogCount;
}

bool TextObjectCtrol::searchRichSegNode(int segLogIndex, int &segPhyIndex, std::string &segStr, std::list<TypeNode> &typeList)
{
	if ((m_pHandleSegNode == nullptr) || (m_pHandleSegNode->getSegLogIndex() != segLogIndex) || (m_pHandleSegNode->getSegState() & (SEG_DELETED|SEG_CANCLE))) {	//m_pHandleSegNodeָ��Ķβ���ָ���ν��
		//�ȵ�ModList�в���
		std::shared_ptr<RichSegNode> pTemp = m_pHeadOfModList;	//��ȡModListͷ���ָ��
		while (pTemp != nullptr) {	//����ModList����
			if (!(pTemp->getSegState()&SEG_DELETED) && !(pTemp->getSegState()&SEG_CANCLE)&&(pTemp->getSegLogIndex() == segLogIndex)) {	//�ҵ�����Ҫ��Ķν��
				segPhyIndex = pTemp->getSegPhyIndex();	//��ȡ����κ�
				segStr = pTemp->getSegStr();	//��ȡ���ַ���
				typeList = pTemp->getTypeList();	//��ȡ�θ�ʽ����ָ��
				return true;
			}
			pTemp = pTemp->getNextSegTextObject();
		}

		//ModList���Ҳ���ת��LineSpace�в���
		if ((segPhyIndex = getPhyIndex(segLogIndex)) != -1) {	//��ȡ����κ�,���߼��κ�תΪ����κ�
			m_pTextObject->getSegmentContent(segPhyIndex, segStr, typeList);	//��LineSpace�л�ȡ������
			return true;
		}

		return false;	//�Ҳ���
	}
	else {	//ֱ����pHandleSegNode�л�ȡ
		segPhyIndex = m_pHandleSegNode->getSegPhyIndex();	//��ȡ����κ�
		segStr = m_pHandleSegNode->getSegStr();	//���ض����ַ���
		typeList = m_pHandleSegNode->getTypeList();	//����TypeListͷָ��
		return true;
	}
}

int TextObjectCtrol::getPhyIndex(int segLogIndex)
{
	if (segLogIndex < 0 || segLogIndex>m_segLogCount)	//�κŲ�����ʱ�˳�
		return -1;
	int segPhyIndex= segLogIndex;	//����κ�
	std::shared_ptr<RichSegNode> pTemp = m_pHeadOfModList;	//��ȡModList��ͷָ��
	while (pTemp != nullptr) {	//��ͷ����ModList����
		if ((pTemp->getSegLogIndex() <= segLogIndex) && (pTemp->getSegState()==SEG_DELETED)) {	//��������ɾ�����,�Ҷκ�С�ڻ���ڵ�ǰsegPhyIndex
			segPhyIndex++;
		}
		else if((pTemp->getSegLogIndex() <= segLogIndex) && (pTemp->getSegState()==SEG_NEW)){
			segPhyIndex--;
		}
		pTemp = pTemp->getNextSegTextObject();	//��ȡ�ϸ����ָ��
	}

	return segPhyIndex;
}

bool TextObjectCtrol::setHandleSegNode(int segLogIndex, int segPhyIndex, const std::string & segStr,const std::list<TypeNode> &typeList, char segState)
{
	m_pHandleSegNode = std::make_shared<RichSegNode>();	//����PreList�½��
	m_pHandleSegNode->setSegLogIndex(segLogIndex);	//�����߼��κ�
	m_pHandleSegNode->setSegPhyIndex(segPhyIndex);	//��������κ�
	m_pHandleSegNode->setSegStr(segStr);	//�����ַ���
	m_pHandleSegNode->setSegState(segState);	//���öν��״̬

	if (m_pHandleSegNode->getSegState()!=SEG_INITAIL) {	//m_pHandleSegNodeԭ�н��״̬����SEGINITALʱ��������ModList
		moveToModList(m_pHandleSegNode);	//��ԭ�еĽ��Ϊ�޸ġ�������ɾ��̬�Ķν�㣬����ת�Ƶ�ModList�ݴ�
	}
	return true;
}

bool TextObjectCtrol::moveToModList(std::shared_ptr<RichSegNode> pNewNode)
{
	//�������뵽ModliSTstǰ�ȸ���ModListԭ�н��
	if (pNewNode->getSegState() == SEG_NEW) {	//�²��뵽ModList�еĽ�����½�̬��������Ҫ����MOdList��ԭ�н����߼��κ�
		std::shared_ptr<RichSegNode> pTemp = m_pHeadOfModList;	//��ȡModList��ͷָ��
		while (pTemp) {	//˳�����ModLIst
			if ((pTemp->getSegLogIndex() > pNewNode->getSegLogIndex()) && !(pTemp->getSegState()&SEG_CANCLE)) {	//ModList�д����߼��κź����������Ȼ��ߴ��ڵĽ��
				pTemp->setSegLogIndex(pTemp->getSegLogIndex() + 1);	//�߼��κż�һ
			}
			pTemp = pTemp->getNextSegTextObject();	//��ȡ�¸�����ָ��
		}
		m_segLogCount++;	//�߼�������һ
	}
	else if (pNewNode->getSegState() == SEG_DELETED) {	//�²��뵽ModLIst�еĽ����ɾ��̬��������Ҫ����ModList��ԭ�н�������κ�
		std::shared_ptr<RichSegNode> pTemp = m_pEndOfModList;	//��ȡModListβ���ָ��
		int dSegIndex=-1;	//������pNewNode�߼��κ���ͬ�Ľ��
		bool isFind = false;	//�ҵ�һ������߼��κź�����ɾ��̬����߼��κ����ʱ��Ϊtrue
		while (pTemp!=nullptr) {	//�������ModList
			if (!isFind && (pTemp->getSegLogIndex() == pNewNode->getSegLogIndex()) && (pTemp->getSegState() == SEG_NEW)) {	//��ModList�д�����pNewNode�߼��κ���ͬ�Ҵ�������LineSpace�е��½ڵ�
				pTemp->setSegState(pTemp->getSegState() | SEG_CANCLE);	//��Ϊȡ��̬�������ý���д��LineSpace
				pNewNode->setSegState(pNewNode->getSegState() | SEG_CANCLE);	//��Ϊȡ��̬����ʾҪɾ���Ľ�㲢û�д�ModListת�Ƶ�LineSpace�У����Բ��õ�LineSpace��ɾ��
				dSegIndex = pTemp->getSegPhyIndex();	//��ȡ�ý�������κ�
				isFind = true;	//�ҵ�һ������߼��κź�����ɾ��̬����߼��κ����ʱ��Ϊtrue
			}
			else if (isFind && (pTemp->getSegPhyIndex() >= dSegIndex) && (pTemp->getSegPhyIndex() != 0)) {	//����ModList�м���dSegIndexָ������pNewNode����н������κ�
				pTemp->setSegPhyIndex(pTemp->getSegPhyIndex() - 1);	//����κż�һ
			}
			pTemp = pTemp->getPreSegTextObject();	//��ȡ�ϸ����ָ��
		}
		//�ڶ���ѭ��ˢ��ModList�е��߼��κ�
		pTemp = m_pHeadOfModList;	
		while (pTemp) {
			if (pTemp->getSegLogIndex() > pNewNode->getSegLogIndex() && !(pTemp->getSegState()&SEG_CANCLE))
				pTemp->setSegLogIndex(pTemp->getSegLogIndex() - 1);	//�߼��κż�һ
			pTemp = pTemp->getNextSegTextObject();
		}
		m_segLogCount--;	//�߼�������һ
	}
	
	//����ModList���½����뵽ModList��
	if (m_pHeadOfModList == nullptr) {	//ModListͷ���Ϊ��ʱ
		m_pHeadOfModList = m_pEndOfModList = m_pBackSegIndex = pNewNode; //ModListͷβָ��ָ���½��
	}
	else {	//�½����뵽ModList��ͷ
		pNewNode->setNextSegTextObject(m_pHeadOfModList);
		m_pHeadOfModList->setPreSegTextObject(pNewNode);
		m_pHeadOfModList = pNewNode;	//�޸�ModListͷָ��
		m_pBackSegIndex = m_pHeadOfModList;	//���³���ָ��
	}
	m_modListCount++;	//ModLIst�ڵ�����һ
	backWriteQuery();	//������ʷ�޸������е�̽�麯��,������д��Ҫ��Ķνڵ�д�뵽TextObject
	return true;
}

int TextObjectCtrol::backWriteQuery()
{
	std::shared_ptr<RichSegNode> pTemp = m_pHeadOfModList;	//��ȡ��ʷ�޸Ķ��е�ͷָ��
	std::shared_ptr<RichSegNode> pPreTemp = pTemp;	//pPreTempָ��pTep���ϸ����
	int backWriteCount = 0;	//��¼���λ�д��ɾ���Ķ���
	while (pTemp != nullptr) {
		int tage = pTemp->getAge();	//��ȡ����ϻ�ֵ
		tage++;	//�ϻ�ֵ��һ
		pTemp->setAge(tage);	//��дpTemp��ageֵ
		if (tage >= MAXAGE) {	//��������ϻ�ֵ
			if (pTemp->getSegState()==SEG_NEW) {	//ModList���½�̬���Զν�㱻�������뵽LineSpace��
				m_pTextObject->insertNewSegmentPost(pTemp->getSegPhyIndex() - 1, pTemp->getSegStr(),pTemp->getTypeList());	//�������뵽LineSpace
				//����ModList�н���������
				std::shared_ptr<RichSegNode> pNode = m_pEndOfModList->getPreSegTextObject();	//��ȡModListβ���ָ����ϸ����
				while (pNode) {
					if ((pNode->getSegPhyIndex() >= pTemp->getSegPhyIndex()) && (pNode->getSegState()==SEG_DELETED))
						pNode->setSegPhyIndex(pNode->getSegPhyIndex() + 1);
					pNode = pNode->getPreSegTextObject();	//��ȡ�ϸ����ָ��
				}
			}
			else if (pTemp->getSegState()==SEG_DELETED) {	//ModList��ɾ��̬�����ڳ�ModList�󣬵�LineSpaceִ��ɾ������
				m_pTextObject->deleteSegment(pTemp->getSegPhyIndex());
				//����ModList�н���������
				std::shared_ptr<RichSegNode> pNode = m_pEndOfModList->getPreSegTextObject();	//��ȡModListβ���ָ���ϸ����
				while (pNode) {
					if ((pNode->getSegPhyIndex() >= pTemp->getSegPhyIndex()) && (pNode->getSegState()==SEG_DELETED))
						pNode->setSegPhyIndex(pNode->getSegPhyIndex() - 1);
					pNode = pNode->getPreSegTextObject();	//��ȡ�ϸ����ָ��
				}
			}
			//�޸�̬�Ľ��ֻ���ڡ�pTempָ��Ķν��Ϊ�ö���ModList�е����һ��������������²Ż�д
			else if (pTemp->getSegLogIndex() != pPreTemp->getSegLogIndex() && (pTemp->getSegState() == SEG_MODIFIED)) {
				m_pTextObject->changeSegment(pTemp->getSegPhyIndex(),  pTemp->getSegStr(),pTemp->getTypeList());
			}

			//���ý�����ʷ�޸Ķ�����ɾ��
			pPreTemp->setNextSegTextObject(pTemp->getNextSegTextObject());
			if (pTemp->getNextSegTextObject()) {	//��ɾ���������¸����
				(pTemp->getNextSegTextObject())->setPreSegTextObject(pPreTemp);	
			}
			if (pTemp == m_pEndOfModList)	//����ɾ����������һ����㣬����m_pEndOfModList
				m_pEndOfModList = pPreTemp;

			m_modListCount--;	//��ʷ�޸�����������һ
			backWriteCount++;	//��ModList���Ƴ��ڵ�����һ
		}
		pPreTemp = pTemp;	//pPreTempָ��pTemp��ָ
		pTemp = pTemp->getNextSegTextObject();	//pTemp��ȡ�¸�����ָ��
	}
	return backWriteCount;	//���ش�ModList���Ƴ��ڵ���
}
