#pragma once
#include "Common.h"
#include "CentralCache.h"

//ThreadCache��һ���̶߳��е���Դ��ֱ���뱾�߳̽���
class ThreadCache
{
public:
	//ThreadCache�ṩ�����ӿڣ�һ��Ϊ�����ڴ棬��һ��Ϊ�ͷ��ڴ�
	void* Allocate(size_t size);
	void Deallocate(void* ptr, size_t size);

	void ReturnToCentralCache(FreeList &freelist, size_t byte);

private:
	//��CentralCache�����ڴ��Ľӿ�,����һ���ڴ�飬��ʣ�µĹ�����ThreadCache�Ķ�Ӧ��
	void* FetchFromCentralCache(size_t index, size_t byte);
private:
	FreeList _freelist[NLISTS];
};