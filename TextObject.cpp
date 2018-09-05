
#include "TextObject.h"


//******************************************************************
bool LineSpaceManage::getLinesContent(int headIndex, std::string &str) const {
	//越界判断
	if (headIndex<0 || headIndex>MAX_LINES - 1) {
		return false;                             
	}
	//获取字符串
	int index = headIndex;
	int count = 0; //计数
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
	//先回收段中块链
	if (headIndex != -1) {
		retrieveLines(headIndex);
	}

	size_t begIndex = 0;	//字符子串起始位置,无符号数
	int newIndex = -1;	//保存新结点下标
	int lastIndex = newIndex;	//保存上个结点下标
								//获取字符子串
	while (begIndex <= str.length()) {
		//当前段为新创建，需要申请内存和连接结点
		if ((newIndex = applyFreeLine()) == -1)
			return false;
		//写入字符串
		strcpy(m_lineSpace[newIndex].charArray, (str.substr(begIndex, MAX_CHARS - 1)).c_str());
		//连接块链结点
		if (lastIndex == -1) {	//当前块链为段内的第一个结点时
			headIndex = newIndex;
		}
		else {
			m_lineSpace[newIndex].llink = lastIndex;
			m_lineSpace[lastIndex].rlink = newIndex;
		}
		lastIndex = newIndex;
		begIndex += MAX_CHARS - 1;	//子字符游标右移
	}
	return true;
}

int LineSpaceManage::hasEmptyLine() const
{
	return (m_freeLineCount != 0);
}

bool LineSpaceManage::initLineSpace()
{
	//初始化块链数组，形成连续的链表结构
	for (int i = 0; i < MAX_LINES; i++) {
		if (i == 0) {	//当创建的是第一个块链时
			m_lineSpace[i].llink = -1;
			m_lineSpace[i].rlink = i + 1;
		}
		else if (i == MAX_LINES - 1) {	//当创建的是最后一个块链
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
	//将新回收的结点插入到空段的第一位
	m_lineSpace[index].llink = -1;
	m_lineSpace[index].rlink = m_freeHeadIndex;
	m_lineSpace[m_freeHeadIndex].llink = index;
	m_freeHeadIndex = index;
	//修改可用行数和已用行数
	m_freeLineCount++;
	m_usedLinesCount--;

	return true;
}

int LineSpaceManage::applyFreeLine()
{
	//没有剩余的块链节点,或者待写入字符串长度大于最大长度(MAX_CHARS-1)
	if (!hasEmptyLine()) {
		return -1;
	}

	int newLine = m_freeHeadIndex;	//获取新块链标号
	m_freeHeadIndex = m_lineSpace[m_freeHeadIndex].rlink;	//修改m_freeHeadIndex
															//修改新加入块链内容
	m_lineSpace[newLine].llink = -1;
	m_lineSpace[newLine].rlink = -1;
	//修改可用行数和已用行数
	m_freeLineCount--;
	m_usedLinesCount++;

	return newLine;
}

bool LineSpaceManage::retrieveLines(int &headIndex)
{
	//遍历所有块链结点并收回
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
	//段中没内容
	if (m_segIndex == -1) {
		return false;
	}
	//调用LineSpaceManage获取段内字符
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
	for (iter = m_TypeList.begin(); iter != m_TypeList.end(); iter++) {	//循环将m_TypeLIst内容拷贝到typeList中
		typeList.push_back(*iter);
	}
	return true;;
}

bool SegElement::setSegTypeList(std::list<TypeNode>& typeList)
{
	m_TypeList.clear();	//清空原有内容
	std::list<TypeNode>::iterator iter;
	for (iter = typeList.begin(); iter != typeList.end(); iter++) {	//循环将typeList内容拷贝到m_TypeList
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
	//判断是否越界
	if (segIndex < -1 || segIndex>m_segCount - 1)
		return false;

	std::shared_ptr<SegElement> pNewSegment = std::make_shared<SegElement>(m_mLineSpaceManage);	//动态创建新段对象，并用智能指针管理																		
	pNewSegment->setSegContent(str);	//写入段字符串
	pNewSegment->setSegTypeList(typeList);	//写入段格式链表

	//插入位置是第一个段之前时，修改m_pHeadOfSegArray																				
	if (segIndex == -1) {
		pNewSegment->setNextSegment(m_pHeadOfSegArray);	//新段结点接入表首位置
		m_pHeadOfSegArray = pNewSegment;	//将当前段指针赋给段表表头指针
		//当插入的是第一个段对象时，给m_pEndOfSegArray赋值
		if (m_segCount == 0)
			m_pEndOfSegArray = pNewSegment;
		m_segCount++;	//段数加一
		return true;
	}
	//当插入的位置是段表的表尾时，不需要遍历链表查找插入点
	else if (segIndex == m_segCount - 1) {
		m_pEndOfSegArray->setNextSegment(pNewSegment);	//把新段结点放置到段表尾
		m_pEndOfSegArray = pNewSegment;	//修改段表表尾指针
		m_segCount++;	//段数加一
		return true;
	}
	//插入位置在段表的中间区域时
	else {
		//遍历段表查找指定位置下标
		std::shared_ptr<SegElement> pSeg;
		if ((pSeg = searchSegment(segIndex)) != nullptr) {	//遍历查找成功
			pNewSegment->setNextSegment(pSeg->getNextSegment());
			pSeg->setNextSegment(pNewSegment);
			m_segCount++;
			return true;
		}
		else    //查找失败
			return false;
	}
}

bool TextObject::deleteSegment(int segIndex)
{
	//越界判断
	if (segIndex<0||segIndex > m_segCount - 1)
		return false;
	std::shared_ptr<SegElement> pSeg;	//临时变量
	//删除第一段时
	if (segIndex == 0) {
		pSeg = m_pHeadOfSegArray;
		pSeg->deleteSegContent();	//释放LineSpace中内存
		m_pHeadOfSegArray = pSeg->getNextSegment();	//头指针指向下个结点
		//若删除的结点是最后一个结点，置m_pEndOfSegArray和m_pHeadOfSegArray都指向空
		if (m_segCount == 1) {
			m_pEndOfSegArray = nullptr;
			m_pHeadOfSegArray = nullptr;
		}
		pSeg = nullptr;//将指针置空，智能指针能自动释放不被任何指针指向的内存
		m_segCount--;	//段数减一
		return true;
	}
	else if ((pSeg = searchSegment(segIndex - 1)) != nullptr) {	//调用searchSegment（）查找指定段结点的上一结点位置，保存到pSeg
		std::shared_ptr<SegElement> pSerhSeg = pSeg->getNextSegment();	//pSerhSeg指向待删除结点
		pSerhSeg->deleteSegContent();	//删除段中指向LineSpace中的内存，段表指针没修改
		pSeg->setNextSegment(pSerhSeg->getNextSegment());	//将SerhSeg结点从段表中删除
		//若删除的是最后一个结点
		if (segIndex == m_segCount - 1) {
			m_pEndOfSegArray = pSerhSeg;	//修改指向段表表尾指针
		}
		pSerhSeg = nullptr;	//将指针置空，智能指针能自动释放不被任何指针指向的内存
		m_segCount--;	//段数减一
		return true;
	}
	else
		return false;
}

bool TextObject::changeSegment(int segIndex, const std::string & str, std::list<TypeNode> &typeList)
{
	//越界判断
	if (segIndex<0 || segIndex>m_segCount)
		return false;

	//调用searchSegment（int）查找指定段
	std::shared_ptr<SegElement> pSeg;
	if ((pSeg = searchSegment(segIndex)) != nullptr) {
		pSeg->setSegContent(str);	//设置段字符串
		pSeg->setSegTypeList(typeList);	//设置段格式链表
		return true;
	}
	else
		return false;
}

bool TextObject::getSegmentContent(int segIndex, std::string & str, std::list<TypeNode> &typeList)
{
	//越界判断
	if (segIndex<0 || segIndex>m_segCount - 1) {
		return false;
	}
	//调用searchSegment（int）查找指定段
	std::shared_ptr<SegElement> pSeg;
	if ((pSeg = searchSegment(segIndex)) != nullptr) {
		pSeg->getSegContent(str);	//获取段字符串
		pSeg->getSegTypeList(typeList);	//获取段格式链表
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
	int count = 0;	//计数块链数
					//遍历找到插入位置下标
	while (count < segIndex&&pSeg != NULL) {
		count++;
		pSeg = pSeg->getNextSegment();
	}

	if (count == segIndex) 	//找到指定段，返回相应指针
		return pSeg;
	else
		return nullptr;
}

bool TextObject::readText(/*CFile *cFile*/)
{	
	

	try {
		const char *filePath = "C:\\Users\\lx109\\Desktop\\MyTextObject.cpp";/*(char *)(cFile->GetFilePath()).GetBuffer();*/	//获取文件路径
		if (filePath == nullptr)
			throw "this File dose not exist!" ;
		std::ifstream  ifile;	//创建文件流
		ifile.open(filePath, std::ios::in | std::ios::_Nocreate);	//打开文本
		if (ifile.is_open()) {	//打开成功
			std::string inStr;
			int segIndex = -1;
			while (getline(ifile, inStr))	//按行读取
			{
				std::list<TypeNode> typeList;	//默认格式链表
				typeList.push_back(TypeNode(TEXTTYPE_SIZE_FOUR | TEXTTYPE_COLOR_BLACK | TEXTTYPE_FONT_SONTI));	//插入全局格式结点

				//inStr.replace(inStr.find('\t'), 1, "    ");	//将制表符转为四个空格
				insertNewSegmentPost(segIndex, inStr, typeList);
				segIndex++;
			}
			return true;
		}
		else {	//打开失败
			throw "Faile to open file";
		}
	}
	catch (const char* msg) {
		CString str(msg);
		str.Format(L"读取数据失败的原因是:%s", msg);
		AfxMessageBox(str);
		return false;
	}
}