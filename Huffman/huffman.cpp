// Haffman.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <map>
#include <wchar.h>
#include <iostream>
#include <locale>
#include <queue>
#include <iterator>
#include <sstream>
#include <fstream>
#include <codecvt>
using namespace std;

std::wstring input = L"天空两块地数据放回两块交水电费两块撒娇地发挥两块噶地方了看见就啊数据库电话费票水电费i暗红色的啥地方了看见嘎斯点击发给阿三间谍飞哥东阳UI的谁gf爱哦无二发给艾欧师傅撒读官方纳斯电焊工佛iU盾房管局二佛iUSA点覅速度规范 ";

typedef std::vector<char> HuffCode;
typedef std::map<wchar_t, HuffCode> HuffCodeMap;

class INode
{
public:
	const int f;
	virtual ~INode(){}
	INode(int f) : f(f){}
};

class InternalNode : public INode
{
public:
	INode* left;
	INode* right;

	InternalNode(INode* c0, INode* c1) : INode(c0->f + c1->f), left(c0), right(c1) {}
	InternalNode() :INode(0), left(NULL), right(NULL){}
	~InternalNode()
	{
		delete left;
		delete right;
	}
};

class LeafNode : public INode
{
public:
	const wchar_t c;

	LeafNode(int f, wchar_t c) : INode(f), c(c) {}
};

struct NodeCmp
{
	bool operator()(const INode* lhs, const INode* rhs) const { return lhs->f > rhs->f; }
};


// 计算字符权值
std::map<wchar_t, int> CalWeight(wchar_t* input, int length)
{
	std::map<wchar_t, int> wmap;
	for (int i = 0; i < length; ++i, ++input)
	{
		wmap.find(*input) == wmap.end() ? wmap[*input] = 1 : wmap[*input] += 1;
	}
	return wmap;
}

// 建立哈夫曼树
INode* BuildTree(std::map<wchar_t, int>& wmap)
{
	std::priority_queue<INode*, std::vector<INode*>, NodeCmp> trees;
	for (std::map<wchar_t, int>::iterator it = wmap.begin(); it != wmap.end(); ++it)
	{
		if (it->second != 0)
		{
			trees.push(new LeafNode(it->second, it->first));
		}
	}
	while (trees.size() > 1)
	{
		INode* childR = trees.top();
		trees.pop();

		INode* childL = trees.top();
		trees.pop();

		INode* parent = new InternalNode(childR, childL);
		trees.push(parent);
	}
	return trees.top();
}

// 重新建立哈夫曼树
INode* BuildTree(HuffCodeMap& dir)
{
	InternalNode* root = new InternalNode();
	for (HuffCodeMap::iterator it = dir.begin(); it != dir.end(); ++it)
	{
		HuffCode codes = it->second;
		// 中间只建立父节点
		InternalNode* sroot = root;
		HuffCode::iterator subit = codes.begin();
		for (; subit != codes.end() - 1; ++subit)
		{
			// 右节点
			if (*subit == 't')
			{
				// 空
				if (sroot->right == NULL)
				{
					InternalNode* node = new InternalNode();
					sroot->right = node;
					sroot = node;
				}
				else
				{
					sroot = (InternalNode*)sroot->right;
				}
			}
			// 左节点
			else
			{
				if (sroot->left == NULL)
				{
					InternalNode* node = new InternalNode();
					sroot->left = node;
					sroot = node;
				}
				else
				{
					sroot = (InternalNode*)sroot->left;
				}
			}

		}
		// 最后建立叶节点,用1表示叶节点
		LeafNode* node = new LeafNode(1, it->first);
		// 右节点
		if (*subit == 't')
		{
			// 空
			if (sroot->right == NULL)
			{
				sroot->right = node;
				sroot = (InternalNode*)node;
			}
			else
			{
				sroot = (InternalNode*)sroot->right;
			}
		}
		// 左节点
		else
		{
			if (sroot->left == NULL)
			{
				sroot->left = node;
				sroot = (InternalNode*)node;
			}
			else
			{
				sroot = (InternalNode*)sroot->left;
			}
		}
	}
	return root;
}

// 生成code
void GenerateCodes(const INode* node, const HuffCode& prefix, HuffCodeMap& outCodes)
{
	if (const LeafNode* lf = dynamic_cast<const LeafNode*>(node))
	{
		outCodes[lf->c] = prefix;
	}
	else if (const InternalNode* in = dynamic_cast<const InternalNode*>(node))
	{
		HuffCode leftPrefix = prefix;
		leftPrefix.push_back('f');
		GenerateCodes(in->left, leftPrefix, outCodes);

		HuffCode rightPrefix = prefix;
		rightPrefix.push_back('t');
		GenerateCodes(in->right, rightPrefix, outCodes);
	}
}

/**
* pOriText wchar* 输入文本的指针，
* OriLen   int    文本长度
* vOutput  vector 输出的字节流(是字节流，不是位流)
* HuffCodeMap map  输出的数据字典
*/
void Compress(wchar_t* pOriText, int OriLen, vector<char>& vOutput, HuffCodeMap& dir)
{
	std::map<wchar_t, int> wmap = CalWeight(pOriText, OriLen);
	INode* root = BuildTree(wmap);
	GenerateCodes(root, HuffCode(), dir);
	std::vector<char> temp(OriLen * 32);// 最坏的情况应该要乘以64,但是乘以32绝对够了
	vOutput.clear();
	// 生成
	int pos = 0;
	char* begin = (char*)&temp[0];
	for (int i = 0; i < OriLen; ++i, ++pOriText)
	{
		std::vector<char> codes = dir[*pOriText];
		int size = codes.size();
		memcpy((begin + pos), codes.data(), size);
		pos += size;
	}
	vOutput.resize(pos);
	memcpy(vOutput.data(), temp.data(), pos);
	delete root;
}

/**
* output   vector       输出的结果
* intput   vcector      原始字节流
* HuffCodeMap  map      输入的数据字典
*/
void DePress(std::vector<wchar_t>& output, vector<char>& input, HuffCodeMap& dir)
{
	output.clear();
	INode* root = BuildTree(dir);
	INode* sroot = root;
	int size = input.size();
	for (int i = 0; i < size; ++i)
	{
		InternalNode* node = (InternalNode*)sroot;
		// 右边
		if (input[i] == 't')
		{
			sroot = node->right;
		}
		else
		{
			sroot = node->left;
		}
		if (sroot->f == 1)
		{
			// 叶节点
			LeafNode* node = (LeafNode*)sroot;
			output.push_back(node->c);
			sroot = root;
		}
	}
}


// 读文件
std::wstring readFile(const char* filename)
{
	
	std::ifstream t("600004.txt");
	std::string str((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());

	unsigned len = str.size() * 4; // 预留字节数
	setlocale(LC_CTYPE, "");       // 必须调用此函数，显示中文
	wchar_t *p = new wchar_t[len]; // 申请一段内存存放转换后的字符串
	mbstowcs(p, str.c_str(), len); // 转换
	std::wstring wstr(p);
	delete[] p;
	return wstr;
}

// 从字节流转二进制位流
void BToBit(std::vector<char>& InputBVec, std::vector<char>& Output, int& BSize, int& BitSize)
{
	Output.clear();
	BitSize = InputBVec.size();
	BSize = BitSize * 1.0 / 8.0 + 0.5;
	for (int i = 0; i < BSize; ++i)
	{
		char temp = 0x00;
		for (int j = 0; j < 8 && (i *8) + j < BitSize; ++j)
		{
			temp = temp << 1;
			temp |= (unsigned int)(InputBVec[i * 8 + j] == 't' ? 1 : 0);
		}
		Output.push_back(temp);
	}
}

// 从位流转化成字节流
void BitToB(std::vector<char>& InputBitVec, std::vector<char>& Output, int& BSize, int& BitSize)
{
	Output.clear();
	Output.resize(BitSize);
	for (int i = 0; i < BSize; ++i)
	{
		char temp = InputBitVec[i];
		for (int j = 0; j < 8 && (i * 8) + j < BitSize; ++j)
		{
			if (temp & 0x80)
				Output[i * 8 + j] = 't';
			else
				Output[i * 8 + j] = 'f';
			temp = temp << 1;
		}
	}
}


int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_CTYPE, "");
	HuffCodeMap codes;
	std::vector<char> res;
	input = readFile("600000.txt");
	//std::wcout<<input<<std::endl;
	std::cout << "压缩开始" << std::endl;
	Compress(&input[0], input.length(), res, codes);
	
	std::vector<char> Bout;
	int BSize = 0;
	int BitSize = 0;
	std::cout << "将字符流转换成二进制位流" << std::endl;
	BToBit(res, Bout, BSize, BitSize);
	std::cout << "转化结束" << std::endl;
	std::cout << "压缩率" << Bout.size() << " / " << input.length() * 2 << " = " << (double)(Bout.size() * 1.0 / (input.size()*2)) << std::endl;
	std::cout << "压缩结束" << std::endl;

	std::cout << "解压缩开始" << std::endl;
	std::vector<char> DeBout;
	std::cout << "二进制位流转化成字符流开始" << std::endl;
	BitToB(Bout, DeBout, BSize, BitSize);
	std::cout << "二进制位流转化成字符流结束" << std::endl;
	std::vector<wchar_t> textres;
	DePress(textres, res, codes);
	std::cout << "解压缩完成" << std::endl;
	std::cout << std::endl;
	getchar();
}

