#include"TextObject.h"
#include"FrontTextObject.h"
#include<iostream>

using std::cout;
using std::cin;
using std::endl;
using std::string;

void menu(std::shared_ptr<TextObjectCtrol> pTextObjectCtrol/*textobject textobject*/) {
	int select = 0;
	int sIndex = -1;
	string str2 = "";
	CString str4;
	char str3[100];
	int begin;
	memset(str3, 0, 100);

	SegStruct segStruct;
	std::list<TypeNode> typeList;

	while (1) {
		cout << "************************" << endl;
		cout << "*��ѡ�����" << endl;
		cout << "*1.�����" << endl;
		cout << "*2.����¶�(post)" << endl;
		cout << "*3.ɾ����" << endl;
		cout << "*4.�޸Ķ�" << endl;
		cout << "*5.��������������ȡ������" << endl;
		cout << "*6.�������" << endl;
		cout << "*7.����" << endl;
		cout << "************************" << endl;
		cin.clear();
		cin >> select;
		switch (select)
		{
		case 0:
			break;
		case 1:
			cin.clear();
			str2 = "";
			cout << "������Ҫ����Ķ�:";
			do {
				cin >> sIndex;
			} while (sIndex < 0);
			cout << endl;
			begin = 0;
			segStruct.segIndex = sIndex-1;
			if (pTextObjectCtrol->userGetSegment(segStruct)) {
				str2 = segStruct.segStr;
				typeList = segStruct.typeList;
				str4 = str2.c_str();
				AfxMessageBox(str4);
			}
			break;
		case 2:
			sIndex = -1;
			str2 = "";
			cin.clear();
			cout << "�������¶ζκ�:" << endl;
			do {
				cin >> sIndex;
			} while (sIndex < 0);
			segStruct.segIndex = sIndex-1;
			cout << "�������¶��ַ���:";
			cin.clear();
			cin >> str3;
			segStruct.segStr = str3;
			pTextObjectCtrol->userAddSegment(segStruct);
			break;
		case 3:

			sIndex = -1;
			cin.clear();
			cout << "����Ҫɾ����";
			do {
				cin >> sIndex;
			} while (sIndex < 0);
			pTextObjectCtrol->userDeleteSegment(sIndex-1);
			break;
		case 4:
			sIndex = -1;
			cin.clear();
			cout << "����Ҫ�޸�";
			do {
				cin >> sIndex;
			} while (sIndex < 1);
			segStruct.segIndex = sIndex-1;
			cout << "����������ַ���:";
			cin.clear();
			cin >> str3;
			str2 = str3;
			segStruct.segStr = str2;
			pTextObjectCtrol->userReviseSegment(segStruct);
			break;
		case 5:
			if (pTextObjectCtrol->userBackSegment(segStruct)) {
				str2 = segStruct.segStr;
				str4 = str2.c_str();
				AfxMessageBox(str4);
			}
			else
				AfxMessageBox(_T("û�п��Գ����Ĳ���!!"));
			
			break;
		case 6:
			str4.Format(_T("%d"), pTextObjectCtrol->getSegLogCount());
			AfxMessageBox(str4);
			 break;
		case 7:
			system("Cls");
			break;
		default:
			break;
		}
	}
}


int main() {

	//TextObject textObject;
	std::shared_ptr<TextObject> pTextObject = std::make_shared<TextObject>();
	std::shared_ptr<TextObjectCtrol> pTextObjectCtrol= std::make_shared<TextObjectCtrol>(pTextObject);
	menu(pTextObjectCtrol/*textObject*/);

	return 1;
}