#ifndef _HEAP_H_
#define _HEAP_H_

#include<iostream>
#include<vector>
#include<assert.h>
using namespace std;





//仿函数
template<class T>
struct Small
{
	bool operator()(const T& left, const T& right)
	{
		return left < right;
	}
};
	
template<class T>
struct Big
{
	bool operator()(const T& left, const T& right)
	{
		return left > right;
	}
};

template<class T,class Compare = Small<T>>
class Heap
{
public:
	Heap()
	{}

	Heap(T* a, size_t n)
	{
		_a.reserve(n);
		for (size_t i = 0; i < n; i++)
		{
			_a.push_back(a[i]);
		}

		//建堆
		for (int i = (_a.size() - 2) / 2; i>=0; --i)
		{//把从第一个非叶子结点的每一个结点都向下调整一遍
			AdjustDown(i);
		}
	}
	//把一个结点向下调整一遍
	void AdjustDown(int root)
	{
		Compare com;
		int Parent = root;
		int Child = 2 * Parent + 1;

		while (Child < _a.size())
		{
			//判断是否有右子树且判断左右子树谁大，若右子树大，则使child指到右子树上(大堆)
			if ((Child + 1)<_a.size() && com(_a[Child+1],_a[Child]))
			{
				Child++;
			}

			if (com(_a[Child],_a[Parent]))
			{
				swap(_a[Parent], _a[Child]);
				Parent = Child;
				Child = 2 * Parent + 1;
			}
			else
			{
				break;
			}
		}
	}

	void AdjustUp(int root)
	{
		Compare com;
		int Child = root;
		int Parent = (Child - 1) / 2;

		while (Parent >= 0)
		{			//向上调整一般用在Push，所以这里在Push之前，已经是一个堆了，所以不用再比较左子树与右子树，因为左子树肯定比父节点小
			if (com(_a[Child],_a[Parent]))
			{
				swap(_a[Parent], _a[Child]);
				Child = Parent;
				Parent = (Child - 1) / 2;
			}
			else//而这里需要break;是因为在Push之前已经是一个堆了，如果发现父节点比当前节点大时，说明当前节点位置已经调好，其他节点不用再调。
			{
				break;
			}
		}
	}


	//先Push到最尾部，然后在向上调整，直至比父节点小，说明调整好了
	void Push(T x)
	{
		_a.push_back(x);
		AdjustUp(_a.size()-1);
	}

	//把堆顶的元素与最后一个元素交换位置，然后从堆顶处向下做一次向下调整
	void Pop()
	{
		swap(_a[0], _a[_a.size() - 1]);
		_a.pop_back();
		AdjustDown(0);
	}

	size_t Size()
	{
		return _a.size();
	}

	bool Empty()
	{
		return _a.empty();
	}

	T& Top()
	{
		assert(_a.size()>0);
		return _a[0];
	}

	bool IsHeap()
	{
		for (size_t i = 0; i < (_a.size() - 2) / 2; i++)
		{
			if (_a[i]<_a[i * 2 + 1] || ((i * 2 + 2)<_a.size() && _a[i]<_a[i * 2 + 2]))
			{
				return false;
			}
		}
		return true;
	}

	bool IsHeapR(int root)
	{
		if (root >= _a.size())
			return true;

		int left = root * 2 + 1;
		int right = root * 2 + 2;
	
		if (left < _a.size())
		{
			if (_a[left]>_a[root])
				return false;

			if (right < _a.size())
			{
				if (_a[right]>_a[root])
					return false;
			}
		}

		return IsHeapR(left) && IsHeapR(right);
	}

protected:
	vector<T> _a;
};

void TestHeap()
{
	int a[] = { 20, 21, 13, 12, 16, 18, 15, 17, 14, 19 };
	Heap<int> hp1(a, sizeof(a) / sizeof(a[0]));

	if (hp1.IsHeapR(0))
	{
		cout << "yes" << endl;
	}
}


#endif 