#pragma once
#include <assert.h> 

template<class T>
class CountingPointer
{
public:
	CountingPointer() : data_(nullptr) {
	}

	CountingPointer(T* data): data_(data) {
		assert(data);
		g_constructed++;
	}

	CountingPointer(const CountingPointer<T>& other) {
		*this = other;
	}

	CountingPointer(CountingPointer<T>&& other) {
		*this = std::move(other);
	}

	~CountingPointer() {
		if (data_) {
			g_destructed++;
			delete data_;
		}
	}

	CountingPointer& operator=(const CountingPointer<T>& other) {
		data_ = new T(*other.data_);
		g_copied++;
		return *this;
	}

	CountingPointer& operator=(CountingPointer<T>&& other) {
		data_ = other.data_;
		other.data_ = nullptr;
		g_moved++;
		return *this;
	}


	static size_t GetConstructedCount() {
		return g_constructed;
	}

	static size_t GetDestructedCount() {
		return g_destructed;
	}

	static size_t GetMovedCount() {
		return g_moved;
	}

	static size_t GetCopiedCount() {
		return g_copied;
	}

	T* Get() {
		return data_;
	}

	const T* Get() const {
		return data_;
	}

	bool operator<(const CountingPointer<T>& other) {
		return *data_ < *other.data_;
	}
private:
	static size_t g_constructed;
	static size_t g_destructed;
	static size_t g_moved;
	static size_t g_copied;

	T* data_;
};

template<typename T>
size_t CountingPointer<T>::g_constructed = 0;

template<typename T>
size_t CountingPointer<T>::g_destructed = 0;

template<typename T>
size_t CountingPointer<T>::g_moved = 0;

template<typename T>
size_t CountingPointer<T>::g_copied = 0;

