#include "PageCache.h"


//������������
PageCache PageCache::_Inst;

Span* PageCache::NewSpan(size_t npage)
{
	//����Եݹ麯�������ķ�ʽ
	std::unique_lock<std::mutex> _lock(_mtx);
	return _NewSpan(npage);
}

Span* PageCache::_NewSpan(size_t npage)
{
	assert(npage < NPAGES);

	//�����ڶ�Ӧ��PageList�ϲ鿴��û�п��е�Span
	if (!_pagelist[npage].Empty())
	{
		//Ŀ��pagelist��Ϊ��
		return _pagelist[npage].Pop();
	}

	//�����Ｔ˵��Ŀ��pagelistΪ�գ���Ҫ����Ѱ�ң����зָ����һ��λ�ÿ�ʼѰ��
	for (size_t i = npage + 1; i < NPAGES; ++i)
	{
		Span* split = nullptr;
		SpanList& pagelist = _pagelist[i];
		if (!pagelist.Empty())
		{
			//�ҵ���pagelist��Ϊ��
			Span* span = pagelist.Pop();

			split = new Span;
			split->_pageid = span->_pageid + span->_npage - npage;	//�Ӻ������ڴ�	�˴�����bug		���ʣ��span����Ӧ�����
			split->_npage = npage;

			//����ʣ����ڴ�
			span->_npage -= npage;

			//��ʣ��Ĵ���ڴ����pageCache����Ӧλ����
			_pagelist[span->_npage].PushFront(span);

			//ֻ������ʹ�õ�ҳ����Ҫ����ӳ��
			for (size_t i = 0; i < split->_npage; ++i)
			{
				//����ҳ��ӳ���ϵ��xxxҳ-xxxҳ��Ϊsplit����
				_id_span_map[split->_pageid + i] = split;
			}
			return split;
		}
	}

	//�ߵ�����˵������û�ÿ��������ָ��Pagelist����Ҫ�������ڴ�����
	void* ptr = VirtualAlloc(NULL, (NPAGES - 1) * 4 * 1024, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);	//ֱ����ϵͳ����128ҳ�Ĵ�С
	if (ptr == nullptr)
	{
		throw std::bad_alloc();
	}
	//����������ڴ������������
	Span* maxspan = new Span;
	maxspan->_objlist = ptr;
	maxspan->_pageid = (PageID)ptr >> 12;
	maxspan->_npage = NPAGES - 1;
	_pagelist[NPAGES - 1].PushFront(maxspan);
	return _NewSpan(npage);
}

Span* PageCache::MapObjectToSpan(void* obj)
{ 
	PageID pageid = (PageID)obj >> 12;	//����ָ�����ڵ�ҳ
	auto it = _id_span_map.find(pageid);	//���Ҹ�ҳ��Ӧ�Ĺ���ҳ

	if(it == _id_span_map.end())
	{
		//��Ӧ�ý�������
		assert(false);
	}
	return it->second;
}

void PageCache::TakeSpanToPageCache(Span* span)
{
	assert(span != nullptr);
	std::unique_lock<std::mutex> lock(_mtx);

	auto previt = _id_span_map.find(span->_pageid - 1);
	while (previt != _id_span_map.end())
	{
		//�鿴ǰһ��Span
		Span* prevspan = previt->second;
		if (prevspan->_usecount != 0)
		{
			//ǰһ��span����ʹ���У�������ǰһ���ϲ�
			break;
		}

		//�ߵ�����˵��������ǰһ��span���кϲ�
		if ((prevspan->_npage + span->_npage) > NPAGES)
		{
			//˵����ǰһ��span�ϲ�֮���ҳ��С����128
			break;
		}

		_pagelist[prevspan->_npage].Earse(prevspan);
		prevspan->_npage += span->_npage;
		delete span;

		span = prevspan;

		//������ǰ�ϲ�
		_id_span_map.erase(previt);
		previt = _id_span_map.find(span->_pageid - 1);

	}

	//���ϲ�
	auto nextit = _id_span_map.find(span->_pageid + span->_npage);
	while (nextit != _id_span_map.end())
	{
		Span* nextspan = nextit->second;
		if ((span->_npage + nextspan->_npage) > NPAGES)
		{
			//����128�����кϲ�
			break;
		}

		if (nextspan->_usecount != 0)
		{
			//ǰһ��span�����в��ϲ�
			break;
		}

		_pagelist[nextspan->_npage].Earse(nextspan);
		span->_npage += nextspan->_npage;
		delete nextspan;

		nextit = _id_span_map.find(span->_pageid + span->_npage);
	}

	//�ϲ���ɣ����¹��أ������½���ӳ��
	for (size_t i = 0; i < span->_npage; ++i)
	{
		_id_span_map[span->_pageid + i] = span;
	}

	_pagelist[span->_npage].PushFront(span);

}
