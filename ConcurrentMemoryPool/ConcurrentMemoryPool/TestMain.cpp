#include "ThreadCache.h"
//#include "Common.h"
#include <iostream>
#include <vector>
#include <thread>
#include "BeachMark.hpp"
using std::cout;
using std::endl;

//���Զ����㷨
void TestRangeupALG(size_t size, size_t align)
{
	cout << "((size + align) / align * align) : "<<((size + align) / align * align) << endl;
	cout << "((size + align - 1) & ~(align - 1)) : " << ((size + align - 1) & ~(align - 1)) << endl;

}

//����freelist�±��㷨
void TestIndexALG(size_t size)
{
	cout << "size: " << size << ";  _index: "<<  ClassSize::Index(size) << endl;
}

void TestAllocate(size_t size)
{
	ThreadCache t;
	std::vector<void*> v_ptr;
	for (size_t i = 0; i < 100; ++i)
	{
		void* p1 = t.Allocate(size);
		cout << p1 << endl;
		v_ptr.push_back(p1);
	}

	for (size_t i = 0; i < v_ptr.size(); ++i)
	{
		cout << v_ptr[i] << " �Ѿ�����" << endl;
		t.Deallocate(v_ptr[i]);
	}
}

int main()
{
	//TestRangeupALG(127, 8);
	//TestIndexALG(1025);
	//TestAllocate(7);
	AHAOAHA::BeachMark(10000, 10000, 3);
	//ThreadCache().Allocate(3);
	


	system("pause");
	return 0;
}