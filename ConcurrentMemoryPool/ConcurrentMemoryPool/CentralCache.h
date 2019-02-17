#pragma once
#include "Common.h"
#include "PageCache.h"
#include <malloc.h>
//��Ϊ������������ֻ����һ��CentralCache����
//���Խ�CentralCache���Ϊ������
class CentralCache
{
public:
	static inline CentralCache* GetInstance()
	{
		return &_Inst;
	}

	size_t FetchRangeObj(void*& start, void*& end, size_t num, size_t byte);
	Span* GetOneSpan(SpanList& spanlist, size_t byte);

private:
	CentralCache() = default;
	CentralCache(const CentralCache&) = delete;
	CentralCache& operator=(const CentralCache&) = delete;
private:
	SpanList _spanlist[NLISTS];

	static CentralCache _Inst;
};
