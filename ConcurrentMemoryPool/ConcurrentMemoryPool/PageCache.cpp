#include "PageCache.h"


//������������
PageCache PageCache::_Inst;

Span* PageCache::NewSpan(size_t npage)
{
	assert(npage < NPAGES);

	//�����ڶ�Ӧ��PageList�ϲ鿴��û�п��е�Span
	if (!_pagelist[npage].Empty())
	{
		//Ŀ��pagelist��Ϊ��
		return _pagelist[npage].Pop();
	}

	//�����Ｔ˵��Ŀ��pagelistΪ�գ���Ҫ����Ѱ�ң����зָ�
	for (size_t i = npage + 1; i < NPAGES; ++i)
	{
		SpanList& pagelist = _pagelist[i];
		if (!pagelist.Empty())
		{
			//�ҵ���pagelist��Ϊ��
			Span* span = pagelist.Pop();
			Span* split = new Span;
			split->_pageid = span->_pageid + span->_npage - npage;
			split->_npage = npage;
			span->_npage -= npage;
			_pagelist[span->_npage].PushFront(span);

			for (size_t i = 0; i < split->_npage; ++i)
			{
				//����ҳ��ӳ���ϵ
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
	return NewSpan(npage);
}
	
	/*Span* newspan = new Span;
	void* ptr = VirtualAlloc(NULL, npage * 4 * 1024, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	newspan->_objlist = ptr;
	newspan->_pageid = (PageID)ptr >> 12;
	newspan->_npage = npage;
	return newspan;*/
