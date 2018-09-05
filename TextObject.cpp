
#include "TextObject.h"


//******************************************************************
bool LineSpaceManage::getLinesContent(int headIndex, std::string &str) const {
	//Խ���ж�
	if (headIndex<0 || headIndex>MAX_LINES - 1) {
		return false;                             
	}
	//��ȡ�ַ���
	int index = headIndex;
	int count = 0; //����
	while (index != -1) {
		str.append(m_lineSpace[index].charArray);
		index = m_lineSpace[index].rlink;
		count++;
	}
	if (count == 0)
		return false;
	else
		return true;
}

bool LineSpaceManage::setLinesContent(int &headIndex, const std::string &str)
{
	//�Ȼ��ն��п���
	if (headIndex != -1) {
		retrieveLines(headIndex);
	}

	size_t begIndex = 0;	//�ַ��Ӵ���ʼλ��,�޷�����
	int newIndex = -1;	//�����½���±�
	int lastIndex = newIndex;	//�����ϸ�����±�
								//��ȡ�ַ��Ӵ�
	while (begIndex <= str.length()) {
		//��ǰ��Ϊ�´�������Ҫ�����ڴ�����ӽ��
		if ((newIndex = applyFreeLine()) == -1)
			return false;
		//д���ַ���
		strcpy(m_lineSpace[newIndex].charArray, (str.substr(begIndex, MAX_CHARS - 1)).c_str());
		//���ӿ������
		if (lastIndex == -1) {	//��ǰ����Ϊ���ڵĵ�һ�����ʱ
			headIndex = newIndex;
		}
		else {
			m_lineSpace[newIndex].llink = lastIndex;
			m_lineSpace[lastIndex].rlink = newIndex;
		}
		lastIndex = newIndex;
		begIndex += MAX_CHARS - 1;	//���ַ��α�����
	}
	return true;
}

int LineSpaceManage::hasEmptyLine() const
{
	return (m_freeLineCount != 0);
}

bool LineSpaceManage::initLineSpace()
{
	//��ʼ���������飬�γ�����������ṹ
	for (int i = 0; i < MAX_LINES; i++) {
		if (i == 0) {	//���������ǵ�һ������ʱ
			m_lineSpace[i].llink = -1;
			m_lineSpace[i].rlink = i + 1;
		}
		else if (i == MAX_LINES - 1) {	//�������������һ������
			m_lineSpace[i].llink = i - 1;
			m_lineSpace[i].rlink = -1;
		}
		else {
			m_lineSpace[i].llink = i - 1;
			m_lineSpace[i].rlink = i + 1;
		}
	}
	return true;
}

bool LineSpaceManage::retrieveFreeLine(int index)
{
	//���»��յĽ����뵽�նεĵ�һλ
	m_lineSpace[index].llink = -1;
	m_lineSpace[index].rlink = m_freeHeadIndex;
	m_lineSpace[m_freeHeadIndex].llink = index;
	m_freeHeadIndex = index;
	//�޸Ŀ�����������������
	m_freeLineCount++;
	m_usedLinesCount--;

	return true;
}

int LineSpaceManage::applyFreeLine()
{
	//û��ʣ��Ŀ����ڵ�,���ߴ�д���ַ������ȴ�����󳤶�(MAX_CHARS-1)
	if (!hasEmptyLine()) {
		return -1;
	}

	int newLine = m_freeHeadIndex;	//��ȡ�¿������
	m_freeHeadIndex = m_lineSpace[m_freeHeadIndex].rlink;	//�޸�m_freeHeadIndex
															//�޸��¼����������
	m_lineSpace[newLine].llink = -1;
	m_lineSpace[newLine].rlink = -1;
	//�޸Ŀ�����������������
	m_freeLineCount--;
	m_usedLinesCount++;

	return newLine;
}

bool LineSpaceManage::retrieveLines(int &headIndex)
{
	//�������п�����㲢�ջ�
	int index = headIndex;
	while (index != -1) {
		int tempIndex = m_lineSpace[index].rlink;
		retrieveFreeLine(index);
		index = tempIndex;
	}
	headIndex = -1;

	return true;
}


//*****************************************************************


bool SegElement::getSegContent(std::string &str) const
{
	//����û����
	if (m_segIndex == -1) {
		return false;
	}
	//����LineSpaceManage��ȡ�����ַ�
	if (m_pLineSpaceManage->getLinesContent(m_segIndex, str)) {
		return true;
	}
	else {
		return false;
	}
}

bool SegElement::setSegContent(const std::string & str)
{
	if (m_pLineSpaceManage->setLinesContent(m_segIndex, str))
		return true;
	else
		return false;
}

bool SegElement::getSegTypeList(std::list<TypeNode>& typeList) 
{
	std::list<TypeNode>::iterator iter;
	for (iter = m_TypeList.begin(); iter != m_TypeList.end(); iter++) {	//ѭ����m_TypeLIst���ݿ�����typeList��
		typeList.push_back(*iter);
	}
	return true;;
}

bool SegElement::setSegTypeList(std::list<TypeNode>& typeList)
{
	m_TypeList.clear();	//���ԭ������
	std::list<TypeNode>::iterator iter;
	for (iter = typeList.begin(); iter != typeList.end(); iter++) {	//ѭ����typeList���ݿ�����m_TypeList
		m_TypeList.push_back(*iter);
	}
	return true;
}

bool SegElement::deleteSegContent()
{
	if (m_pLineSpaceManage->retrieveLines(m_segIndex))
		return true;
	else
		return false;
}

bool SegElement::setNextSegment(std::shared_ptr<SegElement> pSeg)
{
	m_pNextSegment = pSeg;
	return true;
}

std::shared_ptr<SegElement> SegElement::getNextSegment()
{
	return m_pNextSegment;
}

//******************************************************************

bool TextObject::insertNewSegmentPost(int segIndex, const std::string & str, std::list<TypeNode> &typeList)
{
	//�ж��Ƿ�Խ��
	if (segIndex < -1 || segIndex>m_segCount - 1)
		return false;

	std::shared_ptr<SegElement> pNewSegment = std::make_shared<SegElement>(m_mLineSpaceManage);	//��̬�����¶ζ��󣬲�������ָ�����																		
	pNewSegment->setSegContent(str);	//д����ַ���
	pNewSegment->setSegTypeList(typeList);	//д��θ�ʽ����

	//����λ���ǵ�һ����֮ǰʱ���޸�m_pHeadOfSegArray																				
	if (segIndex == -1) {
		pNewSegment->setNextSegment(m_pHeadOfSegArray);	//�¶ν��������λ��
		m_pHeadOfSegArray = pNewSegment;	//����ǰ��ָ�븳���α��ͷָ��
		//��������ǵ�һ���ζ���ʱ����m_pEndOfSegArray��ֵ
		if (m_segCount == 0)
			m_pEndOfSegArray = pNewSegment;
		m_segCount++;	//������һ
		return true;
	}
	//�������λ���Ƕα�ı�βʱ������Ҫ����������Ҳ����
	else if (segIndex == m_segCount - 1) {
		m_pEndOfSegArray->setNextSegment(pNewSegment);	//���¶ν����õ��α�β
		m_pEndOfSegArray = pNewSegment;	//�޸Ķα��βָ��
		m_segCount++;	//������һ
		return true;
	}
	//����λ���ڶα���м�����ʱ
	else {
		//�����α����ָ��λ���±�
		std::shared_ptr<SegElement> pSeg;
		if ((pSeg = searchSegment(segIndex)) != nullptr) {	//�������ҳɹ�
			pNewSegment->setNextSegment(pSeg->getNextSegment());
			pSeg->setNextSegment(pNewSegment);
			m_segCount++;
			return true;
		}
		else    //����ʧ��
			return false;
	}
}

bool TextObject::deleteSegment(int segIndex)
{
	//Խ���ж�
	if (segIndex<0||segIndex > m_segCount - 1)
		return false;
	std::shared_ptr<SegElement> pSeg;	//��ʱ����
	//ɾ����һ��ʱ
	if (segIndex == 0) {
		pSeg = m_pHeadOfSegArray;
		pSeg->deleteSegContent();	//�ͷ�LineSpace���ڴ�
		m_pHeadOfSegArray = pSeg->getNextSegment();	//ͷָ��ָ���¸����
		//��ɾ���Ľ�������һ����㣬��m_pEndOfSegArray��m_pHeadOfSegArray��ָ���
		if (m_segCount == 1) {
			m_pEndOfSegArray = nullptr;
			m_pHeadOfSegArray = nullptr;
		}
		pSeg = nullptr;//��ָ���ÿգ�����ָ�����Զ��ͷŲ����κ�ָ��ָ����ڴ�
		m_segCount--;	//������һ
		return true;
	}
	else if ((pSeg = searchSegment(segIndex - 1)) != nullptr) {	//����searchSegment��������ָ���ν�����һ���λ�ã����浽pSeg
		std::shared_ptr<SegElement> pSerhSeg = pSeg->getNextSegment();	//pSerhSegָ���ɾ�����
		pSerhSeg->deleteSegContent();	//ɾ������ָ��LineSpace�е��ڴ棬�α�ָ��û�޸�
		pSeg->setNextSegment(pSerhSeg->getNextSegment());	//��SerhSeg���Ӷα���ɾ��
		//��ɾ���������һ�����
		if (segIndex == m_segCount - 1) {
			m_pEndOfSegArray = pSerhSeg;	//�޸�ָ��α��βָ��
		}
		pSerhSeg = nullptr;	//��ָ���ÿգ�����ָ�����Զ��ͷŲ����κ�ָ��ָ����ڴ�
		m_segCount--;	//������һ
		return true;
	}
	else
		return false;
}

bool TextObject::changeSegment(int segIndex, const std::string & str, std::list<TypeNode> &typeList)
{
	//Խ���ж�
	if (segIndex<0 || segIndex>m_segCount)
		return false;

	//����searchSegment��int������ָ����
	std::shared_ptr<SegElement> pSeg;
	if ((pSeg = searchSegment(segIndex)) != nullptr) {
		pSeg->setSegContent(str);	//���ö��ַ���
		pSeg->setSegTypeList(typeList);	//���öθ�ʽ����
		return true;
	}
	else
		return false;
}

bool TextObject::getSegmentContent(int segIndex, std::string & str, std::list<TypeNode> &typeList)
{
	//Խ���ж�
	if (segIndex<0 || segIndex>m_segCount - 1) {
		return false;
	}
	//����searchSegment��int������ָ����
	std::shared_ptr<SegElement> pSeg;
	if ((pSeg = searchSegment(segIndex)) != nullptr) {
		pSeg->getSegContent(str);	//��ȡ���ַ���
		pSeg->getSegTypeList(typeList);	//��ȡ�θ�ʽ����
		return true;
	}
	else
		return false;
}

int TextObject::getSegmentCount()
{
	return m_segCount;
}

std::shared_ptr<SegElement> TextObject::searchSegment(int segIndex) const
{
	std::shared_ptr<SegElement> pSeg = m_pHeadOfSegArray;
	int count = 0;	//����������
					//�����ҵ�����λ���±�
	while (count < segIndex&&pSeg != NULL) {
		count++;
		pSeg = pSeg->getNextSegment();
	}

	if (count == segIndex) 	//�ҵ�ָ���Σ�������Ӧָ��
		return pSeg;
	else
		return nullptr;
}

bool TextObject::readText(/*CFile *cFile*/)
{	
	

	try {
		const char *filePath = "C:\\Users\\lx109\\Desktop\\MyTextObject.cpp";/*(char *)(cFile->GetFilePath()).GetBuffer();*/	//��ȡ�ļ�·��
		if (filePath == nullptr)
			throw "this File dose not exist!" ;
		std::ifstream  ifile;	//�����ļ���
		ifile.open(filePath, std::ios::in | std::ios::_Nocreate);	//���ı�
		if (ifile.is_open()) {	//�򿪳ɹ�
			std::string inStr;
			int segIndex = -1;
			while (getline(ifile, inStr))	//���ж�ȡ
			{
				std::list<TypeNode> typeList;	//Ĭ�ϸ�ʽ����
				typeList.push_back(TypeNode(TEXTTYPE_SIZE_FOUR | TEXTTYPE_COLOR_BLACK | TEXTTYPE_FONT_SONTI));	//����ȫ�ָ�ʽ���

				//inStr.replace(inStr.find('\t'), 1, "    ");	//���Ʊ��תΪ�ĸ��ո�
				insertNewSegmentPost(segIndex, inStr, typeList);
				segIndex++;
			}
			return true;
		}
		else {	//��ʧ��
			throw "Faile to open file";
		}
	}
	catch (const char* msg) {
		CString str(msg);
		str.Format(L"��ȡ����ʧ�ܵ�ԭ����:%s", msg);
		AfxMessageBox(str);
		return false;
	}
}