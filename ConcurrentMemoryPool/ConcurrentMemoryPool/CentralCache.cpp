#include "CentralCache.h"

//��������������
CentralCache CentralCache::_Inst;

size_t CentralCache::FetchRangeObj(void*& start, void*& end, size_t num, size_t byte)
{
	//���Դ���
	/*start = malloc(byte * num);
	end = (char*)start + byte * (num - 1);

	void* cur = start;
	while (cur <= end)
	{
		void* next = (char*)cur + byte;
		NEXT_OBJ(cur) = next;

		cur = next;
	}

	NEXT_OBJ(end) = nullptr;

	return num;*/


	assert(byte <= MAXBYTES);

	std::unique_lock<std::mutex> _lock(_mtx);
	size_t index = ClassSize::Index(byte);
	SpanList& spanlist = _spanlist[index];
	size_t fetchnum = 1;
	Span* span = GetOneSpan(spanlist, byte);

	void* prev = nullptr;
	void* cur = span->_objlist;
	while (cur != nullptr && fetchnum < num)
	{
		prev = cur;
		cur = NEXT_OBJ(cur);
		++fetchnum;
	}
	start = span->_objlist;
	end = cur;
	NEXT_OBJ(end) = nullptr;

	span->_usecount += fetchnum;
	return fetchnum;
}

Span* CentralCache::GetOneSpan(SpanList& spanlist, size_t byte)
{
	assert(byte <= MAXBYTES);
	if(!spanlist.Empty())
	{
		//��ǰspanlist��Ϊ��
		return spanlist.Pop();
	}

	//����Ӧ��spanlistΪ��ʱ������Ҫ��PageCache�����µ�spanlist
	size_t npage = ClassSize::NumMovePage(byte);	//������Ҫ��ҳ�Ĵ�С
	Span* newspan = PageCache::GetInstance()->NewSpan(npage);	//��PageCache����µ�Span

	//������õ�Span��������  �˴�span->_objlist��δ��������  ����ҳ�ż����newspan�ĵ�ַ
	char* start = (char*)(newspan->_pageid * 4 * 1024);
	char* end = (start + (newspan->_npage) * 4 * 1024);
	char* cur = start;
	char* next = start + byte;
	while (next < end)
	{
		NEXT_OBJ(cur) = next;
		cur = next;
		next += byte;
	}
	NEXT_OBJ(cur) = nullptr;
	newspan->_objlist = start;
	newspan->_usecount = 0;
	newspan->_objsize = byte;

	//���µ�spanlist������CentralCache��
	spanlist.PushFront(newspan);
	
	return newspan;
}

void CentralCache::ReturnToCentralCache(void* start, size_t byte)
{
	size_t index = ClassSize::Index(byte);
	SpanList &spanlist = _spanlist[index];

	while (start)
	{
		void* next = NEXT_OBJ(start);

		Span* span = PageCache::GetInstance()->MapObjectToSpan(start);
		NEXT_OBJ(start) = span->_objlist;
		span->_objlist = start;

		span->_usecount--;

		if (span->_usecount == 0)
		{
			//˵������ҳspanȫ�����������������ø�span������PageCache
			spanlist.Earse(span);

			span->_objlist = nullptr;
			span->_objsize = 0;
			span->_next = nullptr;
			span->_prev = nullptr;

			PageCache::GetInstance()->TakeSpanToPageCache(span);
			cout << "������PageCache" << endl;
		}
		start = next;
	}
}
