#include "ConcurrentAlloc.h"

_declspec(thread) ThreadCache* _thr_threadCache = nullptr;

void* ConcurrentAlloc(size_t size)
{
	if (size > MAXBYTES)
	{
		//��������ڴ����MAXBYTESʱ����ֱ��ȥpageCache�����ڴ�
		size_t npage = ClassSize::NumMovePage(size);
		return PageCache::GetInstance()->NewSpan(npage);
	}
	
	if (_thr_threadCache != nullptr)
		return _thr_threadCache->Allocate(size);
	_thr_threadCache = new ThreadCache;
	return ConcurrentAlloc(size);
}

void ConcurrentFree(void* ptr, size_t size)
{
	if (_thr_threadCache != nullptr)
	{
		_thr_threadCache->Deallocate(ptr, size);
		return;
	}
	throw Unknown;	//���ͷ��ڴ�ʱ������threadcacheʱ���׳�δ֪�쳣

}

