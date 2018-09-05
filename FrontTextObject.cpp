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
	if (searchRichSegNode(segStruct.segIndex, segPhyIndex, segStr, typeList)) {	//查找段结点
		if (setHandleSegNode(segStruct.segIndex, segPhyIndex, segStr, typeList)) {	//更新当前处理结点
			segStruct.segStr = segStr;	//返回段中字符串
			segStruct.typeList = typeList;	//返回TypeList头指针
			return true;
		}
	}
	return false;
}

bool TextObjectCtrol::userAddSegment(const SegStruct & segStruct)
{
	if (!setHandleSegNode(segStruct.segIndex, segStruct.segIndex, segStruct.segStr, segStruct.typeList,SEG_NEW))	//新建段结点
		return false;								
	return true;
}

bool TextObjectCtrol::userReviseSegment(const SegStruct &segStruct)
{
	int segPhyIndex;
	std::string segStr;
	std::list<TypeNode> typeList;
	if (searchRichSegNode(segStruct.segIndex, segPhyIndex, segStr, typeList)) {	//查找段结点获取物理段号
		if (setHandleSegNode(segStruct.segIndex, segPhyIndex, segStruct.segStr, segStruct.typeList,SEG_MODIFIED)) 	//更新当前处理结点
			return true;
	}
	return false;
}

bool TextObjectCtrol::userDeleteSegment(int segLogIndex)
{
	int segPhyIndex;
	std::string segStr;
	std::list<TypeNode> typeList;
	if (searchRichSegNode(segLogIndex, segPhyIndex, segStr, typeList)) {	//查找要删除的结点是否存在
		if (!setHandleSegNode(segLogIndex, segPhyIndex, segStr, typeList, SEG_DELETED))
			return false;
		return true;
	}
	return false;
}

bool TextObjectCtrol::userBackSegment(SegStruct & segStruct)
{
	if (m_pBackSegIndex) {	//可撤销结点指针不为空且
		while (m_pBackSegIndex&&m_pBackSegIndex->getSegState()&SEG_CANCLE) 	//获取不为SEG_CANCLE态结点
			m_pBackSegIndex = m_pBackSegIndex->getNextSegTextObject();	//置撤销指针指向下个结点
		if (m_pBackSegIndex==nullptr)	//指针为空没有可撤销的结点
			return false;
		if (m_pBackSegIndex->getSegState() == (SEG_DELETED | SEG_CANCLE)) {	//撤销的是删除操作中的特殊情况
			std::shared_ptr<RichSegNode> pTemp = m_pBackSegIndex;
			while (pTemp) {
				if ((pTemp->getSegLogIndex() == m_pBackSegIndex->getSegLogIndex()) && (pTemp->getSegState() == (SEG_CANCLE | SEG_NEW))) 	//获取之前被取消操作的新建态结点指针
					pTemp->setSegState(pTemp->getSegState()&~SEG_CANCLE);	//恢复新增态结点的状态
				pTemp = pTemp->getNextSegTextObject();	//获取下个结点指针
			}
		}
		else {
			m_pBackSegIndex->setSegState(m_pBackSegIndex->getSegState() | SEG_CANCLE);	//一般的结点只要将其状态置为包含SEG_CANCLE即可
		}

		m_pHandleSegNode->setSegState(m_pHandleSegNode->getSegState() | SEG_CANCLE);	//撤销后也要置HandleSegNode为无效态
		//获取被撤销操作的结点内容
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
	if ((m_pHandleSegNode == nullptr) || (m_pHandleSegNode->getSegLogIndex() != segLogIndex) || (m_pHandleSegNode->getSegState() & (SEG_DELETED|SEG_CANCLE))) {	//m_pHandleSegNode指向的段不是指定段结点
		//先到ModList中查找
		std::shared_ptr<RichSegNode> pTemp = m_pHeadOfModList;	//获取ModList头结点指针
		while (pTemp != nullptr) {	//遍历ModList查找
			if (!(pTemp->getSegState()&SEG_DELETED) && !(pTemp->getSegState()&SEG_CANCLE)&&(pTemp->getSegLogIndex() == segLogIndex)) {	//找到满足要求的段结点
				segPhyIndex = pTemp->getSegPhyIndex();	//获取物理段号
				segStr = pTemp->getSegStr();	//获取段字符串
				typeList = pTemp->getTypeList();	//获取段格式链表指针
				return true;
			}
			pTemp = pTemp->getNextSegTextObject();
		}

		//ModList中找不到转到LineSpace中查找
		if ((segPhyIndex = getPhyIndex(segLogIndex)) != -1) {	//获取物理段号,将逻辑段号转为物理段号
			m_pTextObject->getSegmentContent(segPhyIndex, segStr, typeList);	//在LineSpace中获取段内容
			return true;
		}

		return false;	//找不到
	}
	else {	//直接在pHandleSegNode中获取
		segPhyIndex = m_pHandleSegNode->getSegPhyIndex();	//获取物理段号
		segStr = m_pHandleSegNode->getSegStr();	//返回段中字符串
		typeList = m_pHandleSegNode->getTypeList();	//返回TypeList头指针
		return true;
	}
}

int TextObjectCtrol::getPhyIndex(int segLogIndex)
{
	if (segLogIndex < 0 || segLogIndex>m_segLogCount)	//段号不存在时退出
		return -1;
	int segPhyIndex= segLogIndex;	//物理段号
	std::shared_ptr<RichSegNode> pTemp = m_pHeadOfModList;	//获取ModList表头指针
	while (pTemp != nullptr) {	//从头遍历ModList链表
		if ((pTemp->getSegLogIndex() <= segLogIndex) && (pTemp->getSegState()==SEG_DELETED)) {	//遍历到待删除结点,且段号小于或等于当前segPhyIndex
			segPhyIndex++;
		}
		else if((pTemp->getSegLogIndex() <= segLogIndex) && (pTemp->getSegState()==SEG_NEW)){
			segPhyIndex--;
		}
		pTemp = pTemp->getNextSegTextObject();	//获取上个结点指针
	}

	return segPhyIndex;
}

bool TextObjectCtrol::setHandleSegNode(int segLogIndex, int segPhyIndex, const std::string & segStr,const std::list<TypeNode> &typeList, char segState)
{
	m_pHandleSegNode = std::make_shared<RichSegNode>();	//创建PreList新结点
	m_pHandleSegNode->setSegLogIndex(segLogIndex);	//设置逻辑段号
	m_pHandleSegNode->setSegPhyIndex(segPhyIndex);	//设置物理段号
	m_pHandleSegNode->setSegStr(segStr);	//设置字符串
	m_pHandleSegNode->setSegState(segState);	//设置段结点状态

	if (m_pHandleSegNode->getSegState()!=SEG_INITAIL) {	//m_pHandleSegNode原有结点状态不是SEGINITAL时将其推入ModList
		moveToModList(m_pHandleSegNode);	//若原有的结点为修改、新增或删除态的段结点，则将其转移到ModList暂存
	}
	return true;
}

bool TextObjectCtrol::moveToModList(std::shared_ptr<RichSegNode> pNewNode)
{
	//将结点插入到ModliSTst前先更新ModList原有结点
	if (pNewNode->getSegState() == SEG_NEW) {	//新插入到ModList中的结点是新建态，可能需要更新MOdList中原有结点的逻辑段号
		std::shared_ptr<RichSegNode> pTemp = m_pHeadOfModList;	//获取ModList的头指针
		while (pTemp) {	//顺序遍历ModLIst
			if ((pTemp->getSegLogIndex() > pNewNode->getSegLogIndex()) && !(pTemp->getSegState()&SEG_CANCLE)) {	//ModList中存在逻辑段号和新增结点相等或者大于的结点
				pTemp->setSegLogIndex(pTemp->getSegLogIndex() + 1);	//逻辑段号加一
			}
			pTemp = pTemp->getNextSegTextObject();	//获取下个结点的指针
		}
		m_segLogCount++;	//逻辑段数加一
	}
	else if (pNewNode->getSegState() == SEG_DELETED) {	//新插入到ModLIst中的结点是删除态，可能需要更新ModList中原有结点的物理段号
		std::shared_ptr<RichSegNode> pTemp = m_pEndOfModList;	//获取ModList尾结点指针
		int dSegIndex=-1;	//保存与pNewNode逻辑段号相同的结点
		bool isFind = false;	//找到一个结点逻辑段号和新增删除态结点逻辑段号相等时置为true
		while (pTemp!=nullptr) {	//倒序遍历ModList
			if (!isFind && (pTemp->getSegLogIndex() == pNewNode->getSegLogIndex()) && (pTemp->getSegState() == SEG_NEW)) {	//若ModList中存在与pNewNode逻辑段号相同且待新增到LineSpace中的新节点
				pTemp->setSegState(pTemp->getSegState() | SEG_CANCLE);	//置为取消态，不将该结点回写到LineSpace
				pNewNode->setSegState(pNewNode->getSegState() | SEG_CANCLE);	//置为取消态，表示要删除的结点并没有从ModList转移到LineSpace中，所以不用到LineSpace中删除
				dSegIndex = pTemp->getSegPhyIndex();	//获取该结点的物理段号
				isFind = true;	//找到一个结点逻辑段号和新增删除态结点逻辑段号相等时置为true
			}
			else if (isFind && (pTemp->getSegPhyIndex() >= dSegIndex) && (pTemp->getSegPhyIndex() != 0)) {	//更新ModList中夹在dSegIndex指定结点和pNewNode结点中结点物理段号
				pTemp->setSegPhyIndex(pTemp->getSegPhyIndex() - 1);	//物理段号减一
			}
			pTemp = pTemp->getPreSegTextObject();	//获取上个结点指针
		}
		//第二次循环刷新ModList中的逻辑段号
		pTemp = m_pHeadOfModList;	
		while (pTemp) {
			if (pTemp->getSegLogIndex() > pNewNode->getSegLogIndex() && !(pTemp->getSegState()&SEG_CANCLE))
				pTemp->setSegLogIndex(pTemp->getSegLogIndex() - 1);	//逻辑段号减一
			pTemp = pTemp->getNextSegTextObject();
		}
		m_segLogCount--;	//逻辑段数减一
	}
	
	//更新ModList后将新结点插入到ModList中
	if (m_pHeadOfModList == nullptr) {	//ModList头结点为空时
		m_pHeadOfModList = m_pEndOfModList = m_pBackSegIndex = pNewNode; //ModList头尾指针指向新结点
	}
	else {	//新结点插入到ModList表头
		pNewNode->setNextSegTextObject(m_pHeadOfModList);
		m_pHeadOfModList->setPreSegTextObject(pNewNode);
		m_pHeadOfModList = pNewNode;	//修改ModList头指针
		m_pBackSegIndex = m_pHeadOfModList;	//更新撤销指针
	}
	m_modListCount++;	//ModLIst节点数加一
	backWriteQuery();	//调用历史修改链表中的探查函数,对满足写入要求的段节点写入到TextObject
	return true;
}

int TextObjectCtrol::backWriteQuery()
{
	std::shared_ptr<RichSegNode> pTemp = m_pHeadOfModList;	//获取历史修改队列的头指针
	std::shared_ptr<RichSegNode> pPreTemp = pTemp;	//pPreTemp指向pTep的上个结点
	int backWriteCount = 0;	//记录本次回写和删除的段数
	while (pTemp != nullptr) {
		int tage = pTemp->getAge();	//获取结点老化值
		tage++;	//老化值加一
		pTemp->setAge(tage);	//回写pTemp中age值
		if (tage >= MAXAGE) {	//到达最大老化值
			if (pTemp->getSegState()==SEG_NEW) {	//ModList纯新建态属性段结点被真正插入到LineSpace中
				m_pTextObject->insertNewSegmentPost(pTemp->getSegPhyIndex() - 1, pTemp->getSegStr(),pTemp->getTypeList());	//将结点插入到LineSpace
				//更新ModList中结点的物理标号
				std::shared_ptr<RichSegNode> pNode = m_pEndOfModList->getPreSegTextObject();	//获取ModList尾结点指针的上个结点
				while (pNode) {
					if ((pNode->getSegPhyIndex() >= pTemp->getSegPhyIndex()) && (pNode->getSegState()==SEG_DELETED))
						pNode->setSegPhyIndex(pNode->getSegPhyIndex() + 1);
					pNode = pNode->getPreSegTextObject();	//获取上个结点指针
				}
			}
			else if (pTemp->getSegState()==SEG_DELETED) {	//ModList纯删除态结点会在出ModList后，到LineSpace执行删除操作
				m_pTextObject->deleteSegment(pTemp->getSegPhyIndex());
				//更新ModList中结点的物理标号
				std::shared_ptr<RichSegNode> pNode = m_pEndOfModList->getPreSegTextObject();	//获取ModList尾结点指针上个结点
				while (pNode) {
					if ((pNode->getSegPhyIndex() >= pTemp->getSegPhyIndex()) && (pNode->getSegState()==SEG_DELETED))
						pNode->setSegPhyIndex(pNode->getSegPhyIndex() - 1);
					pNode = pNode->getPreSegTextObject();	//获取上个结点指针
				}
			}
			//修改态的结点只有在“pTemp指向的段结点为该段在ModList中的最后一个副本”的情况下才回写
			else if (pTemp->getSegLogIndex() != pPreTemp->getSegLogIndex() && (pTemp->getSegState() == SEG_MODIFIED)) {
				m_pTextObject->changeSegment(pTemp->getSegPhyIndex(),  pTemp->getSegStr(),pTemp->getTypeList());
			}

			//将该结点从历史修改队列中删除
			pPreTemp->setNextSegTextObject(pTemp->getNextSegTextObject());
			if (pTemp->getNextSegTextObject()) {	//被删除结点存在下个结点
				(pTemp->getNextSegTextObject())->setPreSegTextObject(pPreTemp);	
			}
			if (pTemp == m_pEndOfModList)	//若被删除结点是最后一个结点，更新m_pEndOfModList
				m_pEndOfModList = pPreTemp;

			m_modListCount--;	//历史修改链表结点数减一
			backWriteCount++;	//从ModList中移除节点数加一
		}
		pPreTemp = pTemp;	//pPreTemp指向pTemp所指
		pTemp = pTemp->getNextSegTextObject();	//pTemp获取下个结点的指针
	}
	return backWriteCount;	//返回从ModList中移除节点数
}
