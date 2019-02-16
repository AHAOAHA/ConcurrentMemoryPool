#pragma once
#include "Common.h"

//ThreadCache��һ���̶߳��е���Դ��ֱ���뱾�߳̽���
class ThreadCache
{
public:
	//ThreadCache�ṩ�����ӿڣ�һ��Ϊ�����ڴ棬��һ��Ϊ�ͷ��ڴ�
	void* Allocate(size_t size);
	void Deallocate(void* ptr, size_t size);

private:
	FreeList _freelist[NLISTS];
};