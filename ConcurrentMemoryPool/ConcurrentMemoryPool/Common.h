#pragma once
#include <assert.h>

//Common.h��Ҫ�����Ŀ���õ������ݽṹ������

//��ʾ����������һ����240���С���ڴ��
const size_t NLISTS = 240;

//��ʾ���������п��Ը����������ڴ��Ĵ�СΪ64KB
const size_t MAXBYTES = 64 * 1024;

//��ʾPageCache��ҳ��������� �����Ϊ128ҳ 0ҳ���˷�
const size_t NPAGES = 129;

inline void*& NEXT_OBJ(void* ptr)
{
	return *((void**)ptr);
}

class FreeList
{
public:
	bool Empty()
	{
		return _ptr == nullptr;
	}

	void Push(void* obj)
	{
		NEXT_OBJ(obj) = _ptr;
		_ptr = obj;
		++_size;
	}

	void PushRange(void* start, void* end, size_t num)
	{
		NEXT_OBJ(end) = _ptr;
		_ptr = start;
		_size += num;
	}

	void* Pop()
	{
		void* obj = _ptr;
		_ptr = NEXT_OBJ(_ptr);
		return obj;
	}

private:
	void* _ptr = nullptr;

	//��¼�������������ӵ��ڴ���size
	size_t _size = 0;
};

class ClassSize
{
public:
	//align�������ʾ���������ú���������Ϊ����size�����Ӧ�÷�������ڴ��
	static inline size_t _Roundup(size_t size, size_t align)
	{
		//return ((size + align) / align * align);
		return ((size + align - 1) & ~(align - 1));
	}

	//����size�Ĵ�С����Ӧ�ø������ڴ���С
	static inline size_t Roundup(size_t size)
	{
		assert(size <= MAXBYTES);

		//�����Ͻ�freelist��Ϊ�Ķ�	ΪʲôҪ���������Ķ������
		//[8, 128]									8B���� ����STL�ڴ�صķֶι���
		//[129, 1024]							16B����
		//[1025, 8 * 1024]					128B����
		//[8 * 1024 + 1, 64 * 1024]		512B����
		if(size <= 128)
		{
			return _Roundup(size, 8);
		}
		else if(size <= 1024)
		{
			return _Roundup(size, 16);
		}
		else if(size <= 8 * 1024)
		{
			return _Roundup(size, 128);
		}
		else if(size <= 64 * 1024)
		{
			return _Roundup(size, 512);
		}

		//�����ߵ�����ʱ˵��size�Ѿ���Խ�����ڴ�飬���ײ����Ӧ
		assert(false);
		return -1;
	}
	static inline size_t _Index(size_t size, size_t align)
	{
		return _Roundup(size, align) / align - 1;
	}

	static inline size_t Index(size_t size)
	{
		assert(size <= MAXBYTES);

		if(size <= 128)
		{
			return _Index(size, 8);
		}
		else if(size <= 1024)
		{
			return _Index(size - 128, 16) + 16;
		}
		else if(size <= 8 * 1024)
		{
			return _Index(size - 1024, 128) + 16 + 56;
		}
		else if(size <= 64 * 1024)
		{
			return _Index(size - 8 * 1024, 512) + 16 + 56 + 112;
		}

		//���򵽴�ò��裬һ����֮ǰĳ��������
		assert(false);
		return -1;
	}

	//����Ӧ�ø������ٸ��ڴ�飬�ڴ����������[2, 512]֮��
	static size_t NumMoveSize(size_t byte)
	{
		if (byte == 0)
		{
			return 0;
		}
		int num = (int)(MAXBYTES / byte);
		if (num < 2)
			num = 2;

		if (num > 512)
			num = 512;
		return num;
	}

	static size_t NumMovePage(size_t byte)
	{
		//����Ӧ�ø������ٿ��ڴ��
		size_t num = NumMoveSize(byte);
		size_t npage = (size_t)((num * byte) / (4 * 1024));	//��������Ҫ�������ڴ��������Ӧ����Ҫ��ҳ�������ڴ� 1ҳ=4K
		if (npage == 0)
			npage = 1;

		return npage;
	}
};

typedef size_t PageID;
struct Span
{
	PageID _pageid = 0;	// ��¼ҳ��
	size_t _npage = 0;	// ��¼��Span��һ���м�ҳ 1ҳ=4k

	//SpanΪ��ͷ˫��ѭ������
	Span* _prev = nullptr;
	Span* _next = nullptr;

	void* _objlist = nullptr;	// ������������
	size_t _objsize = 0;	//�����ڴ��Ĵ�С
	size_t _usecount = 0;	//�����ڴ��ʵ�ü���
};

//CentralCache�����ݽṹ
class SpanList
{
public:
	SpanList()
	{
		_head = new Span;
		_head->_next = _head;
		_head->_prev = _head;
	}

	bool Empty()
	{
		return _head->_next == _head;
	}

	Span* Pop()
	{
		assert(!Empty());
		Span* span = _head->_next;
		Span* next = span->_next;

		_head->_next = next;
		next->_prev = _head;
		return span;
	}

private:
	Span* _head = nullptr;
};