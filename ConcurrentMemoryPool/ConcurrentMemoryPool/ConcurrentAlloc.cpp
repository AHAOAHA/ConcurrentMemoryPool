#include "ConcurrentAlloc.h"

_declspec(thread) ThreadCache* _thr_threadCache = nullptr;	//��֤threadcacheΪ�̶߳���

void* ConcurrentAlloc(size_t size)
{
	if (size > MAXBYTES)
	{
		//��������ڴ����MAXBYTESʱ����ֱ��ȥpageCache�����ڴ�
		//������ڴ���뵽ҳ
		size_t npage = (size + (4 * 1024)) / (4 * 1024);
		return PageCache::GetInstance()->NewSpan(npage);
	}
	
	if (_thr_threadCache != nullptr)
		return _thr_threadCache->Allocate(size);
	_thr_threadCache = new ThreadCache;
	return ConcurrentAlloc(size);
}

void ConcurrentFree(void* ptr)
{
	if (_thr_threadCache != nullptr)
	{
		_thr_threadCache->Deallocate(ptr);
		return;
	}
	throw Unknown;	//���ͷ��ڴ�ʱ������threadcacheʱ���׳�δ֪�쳣

}

