#include<iostream>
#include<windows.h>
#include"Heap.h"
#include"HuffmanTree.h"
#include"FileCompress.h"


using namespace std;

//测试文件压缩

void TestCompress(char* str)
{
	FileCompress fc1;
	fc1.Compress(str);

}
void TestUnCompress(char* str)
{
	FileCompress fc2;
	fc2.UnCompress(str);
}
enum G                          //枚举类型  
{
	EXIT,
	Compress,
	UnCompress,
};
void menu()          //主界面
{
	printf("!!!******************************************!!!\n");
	printf("!!!**************  Compress    1 ************!!!\n");
	printf("!!!**************  UnCompress  2 ************!!!\n");
	printf("!!!**************  EXIT        0 ************!!!\n");
	printf("!!!******************************************!!!\n");
}


int main()
{

	int key = 0;
	char str[20] = {0};
	do
	{
		menu();
		cout << "请输入选项>";
		cin >> key;
		fflush(stdin);
		switch (key)
		{
		case EXIT:
		{
					 exit(1);
		}
		case Compress:
		{
						 cout << "请输入要压缩的文件名>";
						 scanf("%s", &str);

						 FileCompress fc1;
						 fc1.Compress(str);

						 cout << endl;
						 cout << "压缩完成！"
							 ;
						 system("pause");
						 system("cls");
						 break;
		}
		case UnCompress:
		{
						  cout << "请输入要解压缩的文件名>";
						  scanf("%s", &str);

						   FileCompress fc2;
						   fc2.UnCompress(str);

						   cout << endl;
						   cout << "解压完成！";

						   system("pause");
						   system("cls");
						   break;
		}
		default: printf("<!!无效选项!!>");
			system("pause");
			system("cls");
			break;
		}
	}while(1);

	return 0;
}{\rtf1}