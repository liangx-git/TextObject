#pragma once

#include<List>	//ʹ������


#define TEXTTYPE_NULL 0x00000000	//������
#define TEXTTYPE_EX_ITALIC 0x00000001	//�Ӵ�
#define TEXTTYPE_EX_UNDERLINE 0x00000002	//�»���
#define TEXTTYPE_EX_STRIKEOUT 0x00000004	//ɾ����

#define TEXTTYPE_COLOR_RED	0x00000100	//��ɫ
#define TEXTTYPE_COLOR_BLACK 0x00000200	//��ɫ

#define TEXTTYPE_FONT_SONTI	0x00010000	//����

#define TEXTTYPE_SIZE_FOUR 0x01000000	//�ĺ�����
#define TEXTTYPE_
#define TEXTTYPE_

#define SEG_INITAIL	0x00	//��ʼ̬��
#define SEG_NEW 0x01	//�½�̬
#define SEG_DELETED 0x02	//ɾ��̬
#define SEG_MODIFIED 0x04	//�޸�̬
#define SEG_CANCLE	0x08	//ȡ��̬


#define MAXPRECOUNT 3	//Ԥȡ��������
#define MAXBACKUPCOUNT 5	//��ʷ�޸Ķ������ڵ�����Ҳ�ǳ���������������
#define MAXAGE 5	//����ϻ�ֵ

typedef  unsigned long TEXTTYPE;	//�Զ���������������Ϊ�ĸ��ֽ��޷������������ڿɻ��ɰ˸��ֽ�



//************************************************************************************
//TypeNode��ȷ�����η�Χ�Ľṹ��beg��endȷ�����η�Χ��С��ֵΪ-1��ʾ���ַ���ͷ��β
struct TypeRange {
	TypeRange(int beg = -1, int end = -1) :beg(beg), end(end) {};	//���캯��1
	TypeRange(const TypeRange &typeRange) :beg(typeRange.beg), end(typeRange.end) {};	//���캯��2
	int beg;	//��ʼ�ַ����
	int end;	//�����ַ��±�
};


//************************************************************************************
//��ʽ�����Σ�������
struct TypeNode {
	//���캯��
	TypeNode(TEXTTYPE textType = TEXTTYPE_NULL, TypeRange typeRange = TypeRange(-1, -1)) :
		textType(textType),
		typeRange(typeRange) {};

	TEXTTYPE textType;	//�������ͣ�4�ֽ��޷�������,��λ����ʹ��
	TypeRange typeRange;	//���η�Χ������λλ������
};

