#ifndef CLASS_NCARRAY
#define CLASS_NCARRAY

#include <cstdio> // for NULL
#include <cstdlib> // for exit()
#include <cstring> // for memmove() and memcpy()

/// An array based on templates
template <class T>
class ncArray
{
private:
	T* m_pArray;
	unsigned int m_uSize;
	unsigned int m_uCapacity;
public:
	/// Constructs an array with explicit size
	ncArray(unsigned int uCapacity)
		: m_pArray(NULL), m_uSize(0), m_uCapacity(0)
	{
		SetCapacity(uCapacity);
	}
	~ncArray() { delete[] m_pArray; }

	inline bool isEmpty() const { return m_uSize == 0; }
	inline unsigned int Size() const { return m_uSize; }
	inline unsigned int Capacity() const { return m_uCapacity; }
	void SetCapacity(unsigned int uNewCapacity)
	{
		if (uNewCapacity <= 0)
		{
			ncServiceLocator::GetLogger().Write(1, (char *)"Array::Array - Invalid capacity: %u", uNewCapacity);
			exit(-1);
		}

		T* pNewArray = new T[uNewCapacity];

		if (m_uSize > 0)
		{
			if (uNewCapacity < m_uSize) // shrinking
				m_uSize = uNewCapacity; // cropping last elements

			memcpy(pNewArray, m_pArray, sizeof(T)*m_uSize);
		}

		delete[] m_pArray;
		m_pArray = pNewArray;
		m_uCapacity = uNewCapacity;
	}

	inline void Clear() { m_uSize = 0; }
	void InsertBack(T element) { operator[](m_uSize) = element; }
	void InsertAt(unsigned int uIndex, T element)
	{
		if (uIndex < 0)
		{
			ncServiceLocator::GetLogger().Write(1, (char *)"Array::InsertAt - Invalid element index: %u", uIndex);
			exit(-1);
		}

		if (m_uSize + 1 > m_uCapacity)
			SetCapacity(m_uSize * 2);

		// memmove() takes care of overlapping regions
		memmove(m_pArray + uIndex + 1, m_pArray + uIndex, m_uSize-uIndex);
		m_pArray[uIndex] = element;
		m_uSize++;
	}
	void RemoveAt(unsigned int uIndex)
	{
		if (uIndex < 0)
		{
			ncServiceLocator::GetLogger().Write(1, (char *)"Array::RemoveAt - Invalid element index: %u", uIndex);
			exit(-1);
		}

		// memmove() takes care of overlapping regions
		memmove(m_pArray + uIndex, m_pArray + uIndex + 1, m_uSize-uIndex);
		m_uSize--;
	}

	/// Read-only subscript operator
	const T& operator[] (const unsigned int uIndex) const
	{
		if (uIndex > m_uSize)
		{
			ncServiceLocator::GetLogger().Write(1, (char *)"Array::operator[] - Element %u out of size range!", uIndex);
			exit(-1);
		}

		return m_pArray[uIndex];
	}

	/// Subscript operator
	T& operator[] (const unsigned int uIndex)
	{
		if (uIndex > m_uCapacity - 1)
		{
			ncServiceLocator::GetLogger().Write(1, (char *)"Array::operator[] - Element %u out of capacity range!", uIndex);
//			exit(-1);
		}

		// Avoid creating "holes" into the array
		if (uIndex > m_uSize)
		{
			ncServiceLocator::GetLogger().Write(1, (char *)"Array::operator[] - Element %u out of size range!", uIndex);
			exit(-1);
		}
		// Adding an element at the back of the array
		else if (uIndex == m_uSize)
		{
			// Need growing
			if (m_uSize == m_uCapacity)
				SetCapacity(m_uCapacity * 2);

			m_uSize++;
		}

		return m_pArray[uIndex];
	}
};

#endif
