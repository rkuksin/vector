#include <algorithm>
#include <vector>
#include <iostream>
#include "CountingPointer.h"

template<class T>
using Vector = std::vector<T>;

using IntPointer = CountingPointer<int>;

class VectorOwner {
public:
	// �����������, ����������� ������ �� ��������. C++ 11 style.
	VectorOwner(Vector<IntPointer>&& v) : vector_(std::move(v)) {
		std::cout << "moved vector" << std::endl;
	}
	// �����������, ���������� ������ (�� ���� ���)
	VectorOwner(const Vector<IntPointer>& v) : vector_(v) {
		std::cout << "copied vector" << std::endl;
	}
private:
	Vector<IntPointer> vector_;
};

class VectorOwner_ShorterVersion {
public:
	// �����������, ������� ���� �������� ������ ���� ��������� ��� �� �������� � ����������� �� ����. C++ 11 style.
	VectorOwner_ShorterVersion(Vector<IntPointer> v) : vector_(std::move(v)) {
		std::cout << "moved or copied vector" << std::endl;
	}
private:
	Vector<IntPointer> vector_;
};


int main() {
	{
		Vector<IntPointer> vector;
		// reserve �������� � ������� ������ ��� ��������� ���������� ���������, ����� �� ���� �����������
		vector.reserve(10);

		for (int i : {55, 30, 41, 29, 11}) {
			vector.push_back(new int(i));
		}

		// �������� push_back �������������� � ������ ���� ����� ����������� � ����� ������������ � ������ �������.
		// ������� � ��� 5 �������� ������� � 5 ����������.
		assert(IntPointer::GetConstructedCount() == 5);
		assert(IntPointer::GetMovedCount() == 5);
		assert(IntPointer::GetCopiedCount() == 0);
		assert(IntPointer::GetDestructedCount() == 0);

		for (int i : {43, 46, 13, 15, 8}) {
			vector.emplace_back(new int(i));
		}

		// emplace_back ������� ������ ����� � ������ �������, ������ �� ������������.
		assert(IntPointer::GetConstructedCount() == 10);
		assert(IntPointer::GetMovedCount() == 5);
		assert(IntPointer::GetCopiedCount() == 0);
		assert(IntPointer::GetDestructedCount() == 0);

		// �������� ����� �������
		Vector<IntPointer> vector_copy = vector;

		assert(IntPointer::GetConstructedCount() == 10);
		assert(IntPointer::GetMovedCount() == 5);
		assert(IntPointer::GetCopiedCount() == 10);
		assert(IntPointer::GetDestructedCount() == 0);

		// ����������
		std::sort(vector.begin(), vector.end());

		// ������ ����� ��� ����������� ������ ��� ����� ���������� std::sort.
		// ���������� MS VC++ 17 ���������� � 50 �����������.
		// ���������� �� ���������� � ���� ���������� �� ������ ���� �� ����� �����.
		assert(IntPointer::GetConstructedCount() == 10);
		assert(IntPointer::GetMovedCount() == 55);
		assert(IntPointer::GetCopiedCount() == 10);
		assert(IntPointer::GetDestructedCount() == 0);

		size_t n = 0;
		for (int i : {8, 11, 13, 15, 29, 30, 41, 43, 46, 55}) {
			assert(*vector[n++].Get() == i);
		}

		// ��� ����������� ���������� ������ �� �������� ������� ����� ��������� move-���������:
		VectorOwner owner1(vector);

		// ��� �������� �����������=(
		assert(IntPointer::GetConstructedCount() == 10);
		assert(IntPointer::GetMovedCount() == 55);
		assert(IntPointer::GetCopiedCount() == 20);
		assert(IntPointer::GetDestructedCount() == 0);

		// ��� ��� ������ ���������:
		VectorOwner owner2(std::move(vector));
		
		// �� ������ �����������, �� ����� �����, ��� ���?
		assert(IntPointer::GetConstructedCount() == 10);
		assert(IntPointer::GetMovedCount() == 55);
		assert(IntPointer::GetCopiedCount() == 20);
		assert(IntPointer::GetDestructedCount() == 0);

		// �������� ������ ������������ ������. �� �������� ������ VectorOwner.
		assert(vector.size() == 0);

		// � �������, �� ������� ������� ����� ������� � ����� ��������� �� �� ����� �� ������ ������.
		// ������������ ������:
		VectorOwner_ShorterVersion owner_3(vector_copy);

		// ��� �������� �����������=(
		assert(IntPointer::GetConstructedCount() == 10);
		assert(IntPointer::GetMovedCount() == 55);
		assert(IntPointer::GetCopiedCount() == 30);
		assert(IntPointer::GetDestructedCount() == 0);

		// ���������� ������:
		VectorOwner_ShorterVersion owner_4(std::move(vector_copy));

		// �� ������ ����������� ��� ����� =)
		assert(IntPointer::GetConstructedCount() == 10);
		assert(IntPointer::GetMovedCount() == 55);
		assert(IntPointer::GetCopiedCount() == 30);
		assert(IntPointer::GetDestructedCount() == 0);

		// ���� ������ std::move ��� ����������, ������ � ���.
		assert(vector_copy.size() == 0);	
	}

	// ����� �� ������� �� ������, ��� �������� ������ ���� ����������.
	assert(IntPointer::GetConstructedCount() + IntPointer::GetCopiedCount() == IntPointer::GetDestructedCount());

	system("pause");
	return 0;
}