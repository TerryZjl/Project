#pragma warning(disable:4996)
#include"HuffmanTree.h"
#include<iostream>
#include<string.h>
#include<thread>
using namespace std;

int Judge = 1;

typedef long long Type;
struct CharInfo
{
	unsigned char _ch;
	Type _count;
	string _code;

	bool operator!=(const CharInfo& ci) const
	{
		return _count != ci._count;
	}

	CharInfo operator+(const CharInfo& ci) const
	{
		CharInfo ret;
		ret._count = _count + ci._count;
		return ret;
	}
	bool operator<(const CharInfo& ci) const
	{
		return _count < ci._count;
	}
};

struct CountInfo
{
	unsigned char _ch;
	size_t _count;
};

void ring()
{
	char ch[] = { '|', '/', '-', '\\' };
	size_t  i = 0;
	while (Judge)
	{
		printf("[%c]\r", ch[i%4]);
		i++;
		fflush(stdout);
		Sleep(2);
	}
}

void Progress(int begin, int end)
{
	char buffer[52] = { '\0' };
	int i = begin;
	for (int j = 0; j < begin; j++)
	{
		buffer[j] = '#';
	}
	while (i <= end)
	{
		printf("   [%-50s][%d%%]\r", buffer, 2 * i);
		fflush(stdout);
		buffer[i] = '#';
		i++;
		buffer[i] = '\0';
		Sleep(2);
	}
}

class FileCompress
{
	typedef HuffmanTreeNode<CharInfo> Node;
public:

	FileCompress()
	{
		//将每个素组元素赋成相应的字符，且把出现的次数置成0
		for (size_t i = 0; i < 256; ++i)
		{
			_info[i]._ch = i;
			_info[i]._count = 0;
		}
	}

	void Compress(char* Filename)
	{
		thread ri(ring);
		thread Pro(Progress,0,10);
		//统计字符出现的次数
		FILE* fout = fopen(Filename, "rb");
		//char ch = fgetc(fout);
		unsigned char ch;
		fread(&ch, sizeof(ch), 1, fout);

		while (!feof(fout))
		{
			_info[ch]._count++;
			fread(&ch, sizeof(ch), 1, fout);
			//ch = fgetc(fout);
		}

		Pro.join();
		thread Pro1(Progress, 10,20);

		//构建Huffman树
		CharInfo ivaliad;
		ivaliad._count = 0;
		HuffmanTree<CharInfo> tree(_info, 256, ivaliad);

		//生成Huffman编码
		GetHuffmanCode(tree.GetRoot());

		Pro1.join();
		thread Pro2(Progress, 20, 30);
		
		//压缩
		string str1 = Filename;
		size_t pos = str1.rfind('.');
		assert(pos != string::npos);
		string str2 = str1.substr(0, pos) +".hfm";
		FILE* fin = fopen(str2.c_str(), "wb");
		assert(fin);
		////编写配置文件
		CountInfo info;
		for (size_t i = 0; i < 256; ++i)
		{
			if (_info[i] != ivaliad)
			{
				info._ch = _info[i]._ch;
				info._count = _info[i]._count;
				fwrite(&info, sizeof(info), 1, fin);
			}
		}

		info._count = -1;
		fwrite(&info, sizeof(info), 1, fin);
		
		Pro2.join();
		thread Pro3(Progress, 30,40);

		//将Huffman code写入压缩文件
		fseek(fout, 0, SEEK_SET);
		char value = 0; 
		int count = 0;    //记录够八位就写一次
		//ch = fgetc(fout);
		fread(&ch,sizeof(ch),1,fout);
		while (!feof(fout))
		{
			string& code = _info[ch]._code;
			for (size_t i = 0; i < code.size();++i)
			{
				value <<= 1;
				count++;
				if (code[i] == '1')
				{
					value |= 1;
				}
				if (count == 8)
				{
					//fputc(value, fin);
					fwrite(&value, sizeof(value), 1, fin);
					value = 0;
					count = 0;
				}
			}
			//ch = fgetc(fout);
			fread(&ch, sizeof(ch), 1, fout);
		}
		if (count != 0)
		{
			value <<= (8 - count);
			//fputc(value, fin);
			fwrite(&value, sizeof(value), 1, fin);
		}
		Pro3.join();
		thread Pro4(Progress, 40, 50);

		fclose(fin);
		fclose(fout);

		Pro4.join();

		Judge = 0;
		ri.join();
		Judge = 1;
	} 

	void UnCompress(char* Filename)
	{
		thread ri(ring);
		thread Pro(Progress,0,10);
		//读取配置文件，统计字符次数
		FILE* fout = fopen(Filename, "rb");
		assert(fout);
		CountInfo info;
		fread(&info, sizeof(info), 1, fout);
		while(info._count!=-1)
		{
			if (info._count != 0)
			{
				_info[info._ch]._ch = info._ch;
				_info[info._ch]._count = info._count;
				fread(&info, sizeof(info), 1, fout);
			}
		}
		Pro.join();
		thread Pro1(Progress, 10, 20);
		//重建Huffman树
		CharInfo ivaliad;
		ivaliad._count = 0;
		HuffmanTree<CharInfo> tree(_info, 256, ivaliad);

		Pro1.join();
		thread Pro2(Progress, 20, 35);
		//解压
		Node* root = tree.GetRoot();
		int count = root->_w._count;
		Node* cur = root;
		string str1 = Filename;
		size_t pos = str1.rfind('.');
		assert(pos != string::npos);
		string str2 = str1.substr(0,pos) + ".uhfm";
		FILE* fin = fopen(str2.c_str(), "wb");
		assert(fin);

		//从压缩文件中读取一个字符，这字符的位里面存的就是Huffman编码，所以读取每一位编码，从HuffmanTree根结点往下找，找到叶子结点，写入解压缩文件即可。
		unsigned char ch = getc(fout);
		if (feof(fout))  //如果文件里只有一种字符，Huffman树里只有根结点，压缩文件里就没有相对应的code
		{
			for (size_t i = count; i >0 ; --i)
			{
				ch = fputc(root->_w._ch, fin);
			}
		}
		else
		{
			while (count)
			{
				for (int i = 7; i >= 0; --i)
				{
					if ((ch&(1 << i)) == 0)
					{
						cur = cur->_left;
					}
					else
					{
						cur = cur->_right;
					}
					if ((cur->_right == NULL) && (cur->_left == NULL))
					{
						fputc(cur->_w._ch, fin);
						cur = root;
						count--;
						if (count <= 0)
						{
							break;
						}
					}
				}
				ch = fgetc(fout);
			}
		}
		Pro2.join();
		thread Pro3(Progress, 35,50);

		fclose(fin);
		fclose(fout);

		Pro3.join();

		Judge = 0;
		ri.join();
		Judge = 1;


	}

private:
	Node* GetHuffmanCode(Node* root)
	{
		if (root == NULL)
			return NULL;

		if ((root->_left == NULL)&&(root->_right == NULL))
		{
			string code;
			Node* parent = root->_parent;
			Node* cur = root;
			while (parent)
			{
				if (parent->_left == cur)
				{
					code.push_back('0');
				}

				if (parent->_right == cur)
				{
					code.push_back('1');
				}
				cur = parent;
				parent = cur->_parent;
			}
			reverse(code.begin(),code.end());
			_info[root->_w._ch]._code = code;
		}
		GetHuffmanCode(root->_left);
		GetHuffmanCode(root->_right);
	}
private:
	CharInfo _info[256];
};
