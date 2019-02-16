#pragma once
#include <assert.h>

//Common.h��Ҫ�����Ŀ���õ������ݽṹ������

//��ʾ����������һ����240���С���ڴ��
const size_t NLISTS = 240;

//��ʾ���������п��Ը����������ڴ��Ĵ�СΪ64KB
const size_t MAXBYTES = 64 * 1024 * 1024;

void*& NEXT_OBJ(void* ptr)
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
			_Roundup(size, 8);
		}
		else if(size <= 1024)
		{ }
		else if(size <= 8 * 1024)
		{ }
		else if(size < 64 * 1024)
		{ }

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
};