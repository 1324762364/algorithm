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

std::wstring input = L"�����������ݷŻ����齻ˮ������������ط���������ط��˿����Ͱ����ݿ�绰��Ʊˮ���i����ɫ��ɶ�ط��˿�����˹���������������ɸ綫��UI��˭gf��Ŷ�޶�������ŷʦ�������ٷ���˹�纸����iU�ܷ��ֶܾ���iUSA��҅�ٶȹ淶 ";

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


// �����ַ�Ȩֵ
std::map<wchar_t, int> CalWeight(wchar_t* input, int length)
{
	std::map<wchar_t, int> wmap;
	for (int i = 0; i < length; ++i, ++input)
	{
		wmap.find(*input) == wmap.end() ? wmap[*input] = 1 : wmap[*input] += 1;
	}
	return wmap;
}

// ������������
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

// ���½�����������
INode* BuildTree(HuffCodeMap& dir)
{
	InternalNode* root = new InternalNode();
	for (HuffCodeMap::iterator it = dir.begin(); it != dir.end(); ++it)
	{
		HuffCode codes = it->second;
		// �м�ֻ�������ڵ�
		InternalNode* sroot = root;
		HuffCode::iterator subit = codes.begin();
		for (; subit != codes.end() - 1; ++subit)
		{
			// �ҽڵ�
			if (*subit == 't')
			{
				// ��
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
			// ��ڵ�
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
		// �����Ҷ�ڵ�,��1��ʾҶ�ڵ�
		LeafNode* node = new LeafNode(1, it->first);
		// �ҽڵ�
		if (*subit == 't')
		{
			// ��
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
		// ��ڵ�
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

// ����code
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
* pOriText wchar* �����ı���ָ�룬
* OriLen   int    �ı�����
* vOutput  vector ������ֽ���(���ֽ���������λ��)
* HuffCodeMap map  ����������ֵ�
*/
void Compress(wchar_t* pOriText, int OriLen, vector<char>& vOutput, HuffCodeMap& dir)
{
	std::map<wchar_t, int> wmap = CalWeight(pOriText, OriLen);
	INode* root = BuildTree(wmap);
	GenerateCodes(root, HuffCode(), dir);
	std::vector<char> temp(OriLen * 32);// ������Ӧ��Ҫ����64,���ǳ���32���Թ���
	vOutput.clear();
	// ����
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
* output   vector       ����Ľ��
* intput   vcector      ԭʼ�ֽ���
* HuffCodeMap  map      ����������ֵ�
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
		// �ұ�
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
			// Ҷ�ڵ�
			LeafNode* node = (LeafNode*)sroot;
			output.push_back(node->c);
			sroot = root;
		}
	}
}


// ���ļ�
std::wstring readFile(const char* filename)
{
	
	std::ifstream t("600004.txt");
	std::string str((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());

	unsigned len = str.size() * 4; // Ԥ���ֽ���
	setlocale(LC_CTYPE, "");       // ������ô˺�������ʾ����
	wchar_t *p = new wchar_t[len]; // ����һ���ڴ���ת������ַ���
	mbstowcs(p, str.c_str(), len); // ת��
	std::wstring wstr(p);
	delete[] p;
	return wstr;
}

// ���ֽ���ת������λ��
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

// ��λ��ת�����ֽ���
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
	std::cout << "ѹ����ʼ" << std::endl;
	Compress(&input[0], input.length(), res, codes);
	
	std::vector<char> Bout;
	int BSize = 0;
	int BitSize = 0;
	std::cout << "���ַ���ת���ɶ�����λ��" << std::endl;
	BToBit(res, Bout, BSize, BitSize);
	std::cout << "ת������" << std::endl;
	std::cout << "ѹ����" << Bout.size() << " / " << input.length() * 2 << " = " << (double)(Bout.size() * 1.0 / (input.size()*2)) << std::endl;
	std::cout << "ѹ������" << std::endl;

	std::cout << "��ѹ����ʼ" << std::endl;
	std::vector<char> DeBout;
	std::cout << "������λ��ת�����ַ�����ʼ" << std::endl;
	BitToB(Bout, DeBout, BSize, BitSize);
	std::cout << "������λ��ת�����ַ�������" << std::endl;
	std::vector<wchar_t> textres;
	DePress(textres, res, codes);
	std::cout << "��ѹ�����" << std::endl;
	std::cout << std::endl;
	getchar();
}

