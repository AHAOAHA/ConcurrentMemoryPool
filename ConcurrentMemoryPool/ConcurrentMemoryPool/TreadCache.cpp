#include "ThreadCache.h"

//����ռ�
void* ThreadCache::Allocate(size_t size)
{
	//Ԥ����ʩ����ֹ��Ҫ�������ڴ���ڿ��Ը���������С
	assert(size <= MAXBYTES);

	//�����û���Ҫsize�Ĵ�С����ThreadCacheӦ�ø������ڴ���С
	size = ClassSize::Roundup(size);

	//�����ڴ��Ĵ�С������ڴ����������������±�
	size_t index = ClassSize::Index(size);
	FreeList& freelist = _freelist[index];
	if (!freelist.Empty())
	{
		//��ʾ�ô������������º��п��õ��ڴ��
		return freelist.Pop();
	}

	//�ߵ��ô���˵������������û�п��õ��ڴ��
	return FetchFromCentralCache(index, size);
}

void* ThreadCache::FetchFromCentralCache(size_t index, size_t byte)
{
	assert(byte <= MAXBYTES);
	FreeList& freelist = _freelist[index];
	size_t num = 10;	//��Ҫ��CentralCache�õ����ڴ��ĸ���

	void *start, *end;	//����õ����ڴ�	fetchnum��ʾ��ʵ�õ����ڴ�����
	size_t fetchnum = CentralCache::GetInstance()->FetchRangeObj(start, end, num, byte);
	if (fetchnum == 1)
	{
		//���ֻ��CentralCache�õ�һ��Ͳ��ý�ʣ����ڴ�����������������
		return start;
	}

	freelist.PushRange(NEXT_OBJ(start), end, fetchnum - 1);
	return start;
}

//�ͷ��ڴ��
void ThreadCache::Deallocate(void* ptr, size_t size)
{
	size = ClassSize::Roundup(size);
	size_t index = ClassSize::Index(size);
	FreeList& freelist = _freelist[index];

	//���ڴ��ͷ��
	freelist.Push(ptr);
}