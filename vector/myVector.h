#pragma once
#include <assert.h>

template <class T> class myVector
{
public:
	myVector();
	myVector(const myVector& other);
	myVector(myVector&& other);
	void reserve(size_t size);
	void push_back(const T& value);
	void push_back(T&& value);
	template<class...Args>
	void emplace_back(Args...args) {
		assert(dataSize >= length);
		if (length == dataSize)
		{
			this->reserve(dataSize * 2);
		}
		new(data + length)T(args...);
		length++;
	}
	size_t size();
	T* begin() { return data; };
	T* end() { return data + length; };
	T& operator[] (const size_t n) { 
		return *(data + n); 
	}
	~myVector();
private:
	T* data;
	size_t dataSize;
	size_t length;
	//std::iterator:: it;
};

template <class T> myVector<T>::myVector() : data(nullptr), dataSize(0), length(0) {}

template <class T> myVector<T>::myVector(const myVector& other) : dataSize(other.dataSize), length(other.length)
{
	data = (T*)calloc(other.dataSize, sizeof(T));
	for (size_t i = 0; i < other.length; i++)
	{
		new(data + i)T(*(other.data + i));
	}
	//it = data;
}

template <class T> myVector<T>::myVector(myVector&& other)
{
	data = other.data;
	length = other.length;
	dataSize = other.dataSize;
	//it = other.it;
	other.length = 0;
	other.dataSize = 0;
	other.data = nullptr;
}
template <class T> void myVector<T>::reserve(size_t size)
{
	if (dataSize < size)
	{
		dataSize = size;
		T* temp = data;
		data = (T*)calloc(size, sizeof(T));
		for (size_t i = 0; i <length; i++)
		{
			new(data + i)T(*(temp + i));
			(temp + i)->~T();
		}
		free(temp);
	}
}

template <class T> void myVector<T>::push_back(const T& value)
{
	assert(dataSize >= length);
	if (length == dataSize)
	{
		this->reserve(dataSize * 2);
	}
	new(data + length)T(value);
	length++;
}

template <class T> void myVector<T>::push_back(T&& value)
{
	assert(dataSize >= length);
	if (length == dataSize)
	{
		this->reserve(dataSize * 2);
	}
	new(data + length)T(std::move(value));
	length++;
}


template <class T> size_t myVector<T>::size()
{
	return length;
}

template <class T> myVector<T>::~myVector()
{
	for (size_t i = 0; i < length; i++)
	{
		(data + i)->~T();
	}
	free(data);
}

/*template <class T> T operator[] (const size_t n)
{
	return *(data + n);
}

template <class T> T* begin()
{
	return data;
}

template <class T> T* end()
{
	return data + length;
}*/