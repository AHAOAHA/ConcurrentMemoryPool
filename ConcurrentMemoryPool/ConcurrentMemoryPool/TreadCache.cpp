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
	
	return nullptr;
}