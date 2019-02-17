#include "ThreadCache.h"
//#include "Common.h"
#include <iostream>
#include <vector>
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
	t.Allocate(size);
}
int main()
{
	//TestRangeupALG(127, 8);
	//TestIndexALG(1025);
	TestAllocate(8);
	return 0;
}