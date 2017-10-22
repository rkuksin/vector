#include <algorithm>
#include <vector>
#include <iostream>
#include "CountingPointer.h"

template<class T>
using Vector = std::vector<T>;

using IntPointer = CountingPointer<int>;

class VectorOwner {
public:
	// Конструктор, принимающий вектор во владение. C++ 11 style.
	VectorOwner(Vector<IntPointer>&& v) : vector_(std::move(v)) {
		std::cout << "moved vector" << std::endl;
	}
	// Конструктор, копирующий вектор (на надо так)
	VectorOwner(const Vector<IntPointer>& v) : vector_(v) {
		std::cout << "copied vector" << std::endl;
	}
private:
	Vector<IntPointer> vector_;
};

class VectorOwner_ShorterVersion {
public:
	// Конструктор, который либо копирует вектор либо принимает его во владение в зависимости от типа. C++ 11 style.
	VectorOwner_ShorterVersion(Vector<IntPointer> v) : vector_(std::move(v)) {
		std::cout << "moved or copied vector" << std::endl;
	}
private:
	Vector<IntPointer> vector_;
};


int main() {
	{
		Vector<IntPointer> vector;
		// reserve выделяет в векторе память под указанное количество элементов, чтобы не было реаллокации
		vector.reserve(10);

		for (int i : {55, 30, 41, 29, 11}) {
			vector.push_back(new int(i));
		}

		// Аргумент push_back конвертируется к нашему типу через конструктор а потом перемещается в память вектора.
		// Поэтому у нас 5 объектов создано и 5 перемещено.
		assert(IntPointer::GetConstructedCount() == 5);
		assert(IntPointer::GetMovedCount() == 5);
		assert(IntPointer::GetCopiedCount() == 0);
		assert(IntPointer::GetDestructedCount() == 0);

		for (int i : {43, 46, 13, 15, 8}) {
			vector.emplace_back(new int(i));
		}

		// emplace_back создает объект прямо в памяти вектора, ничего не перемещается.
		assert(IntPointer::GetConstructedCount() == 10);
		assert(IntPointer::GetMovedCount() == 5);
		assert(IntPointer::GetCopiedCount() == 0);
		assert(IntPointer::GetDestructedCount() == 0);

		// создадим копию вектора
		Vector<IntPointer> vector_copy = vector;

		assert(IntPointer::GetConstructedCount() == 10);
		assert(IntPointer::GetMovedCount() == 5);
		assert(IntPointer::GetCopiedCount() == 10);
		assert(IntPointer::GetDestructedCount() == 0);

		// сортировка
		std::sort(vector.begin(), vector.end());

		// Скорее всего это справедливо только для одной реализации std::sort.
		// Реализация MS VC++ 17 справилась в 50 перемещений.
		// Независимо от реализации в ходе сортировки не должно быть ни одной копии.
		assert(IntPointer::GetConstructedCount() == 10);
		assert(IntPointer::GetMovedCount() == 55);
		assert(IntPointer::GetCopiedCount() == 10);
		assert(IntPointer::GetDestructedCount() == 0);

		size_t n = 0;
		for (int i : {8, 11, 13, 15, 29, 30, 41, 43, 46, 55}) {
			assert(*vector[n++].Get() == i);
		}

		// Как неправильно передавать вектор во владение объекту после появления move-семантики:
		VectorOwner owner1(vector);

		// все элементы скопированы=(
		assert(IntPointer::GetConstructedCount() == 10);
		assert(IntPointer::GetMovedCount() == 55);
		assert(IntPointer::GetCopiedCount() == 20);
		assert(IntPointer::GetDestructedCount() == 0);

		// Как это делать правильно:
		VectorOwner owner2(std::move(vector));
		
		// Ни одного перемещения, ни одной копии, как так?
		assert(IntPointer::GetConstructedCount() == 10);
		assert(IntPointer::GetMovedCount() == 55);
		assert(IntPointer::GetCopiedCount() == 20);
		assert(IntPointer::GetDestructedCount() == 0);

		// Вектором больше пользоваться нельзя. Он переехал внутрь VectorOwner.
		assert(vector.size() == 0);

		// К счастью, мы заранее сделали копию вектора и можем проверить то же самое на втором классе.
		// Неправильный способ:
		VectorOwner_ShorterVersion owner_3(vector_copy);

		// все элементы скопированы=(
		assert(IntPointer::GetConstructedCount() == 10);
		assert(IntPointer::GetMovedCount() == 55);
		assert(IntPointer::GetCopiedCount() == 30);
		assert(IntPointer::GetDestructedCount() == 0);

		// Правильный способ:
		VectorOwner_ShorterVersion owner_4(std::move(vector_copy));

		// ни одного перемещения или копии =)
		assert(IntPointer::GetConstructedCount() == 10);
		assert(IntPointer::GetMovedCount() == 55);
		assert(IntPointer::GetCopiedCount() == 30);
		assert(IntPointer::GetDestructedCount() == 0);

		// Если видишь std::move для переменной, забудь о ней.
		assert(vector_copy.size() == 0);	
	}

	// Когда мы выходим из скоупа, все элементы должны быть уничтожены.
	assert(IntPointer::GetConstructedCount() + IntPointer::GetCopiedCount() == IntPointer::GetDestructedCount());

	system("pause");
	return 0;
}