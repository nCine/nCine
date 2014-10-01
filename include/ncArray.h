#ifndef CLASS_NCARRAY
#define CLASS_NCARRAY

#include <cstdio> // for NULL
#include <cstdlib> // for exit()
#include <cstring> // for memmove() and memcpy()
#include "ncServiceLocator.h"

/// An array based on templates
template <class T>
class ncArray
{
 public:
	/// Constructs an array with explicit size
	ncArray(unsigned int uCapacity)
		: m_pArray(NULL), m_uSize(0), m_uCapacity(0)
	{
		SetCapacity(uCapacity);
	}
	~ncArray() { delete[] m_pArray; }

	/// Returns true if the array is empty
	inline bool isEmpty() const { return m_uSize == 0; }
	/// Returns the array size
	/** The array is filled without gaps until the Size()-1 element */
	inline unsigned int Size() const { return m_uSize; }
	/// Returns the array size
	/// The array has memory allocated to store untile the Capacity()-1 element
	inline unsigned int Capacity() const { return m_uCapacity; }
	// Set a new capacity for the array (can be bigger or smaller than the current one)
	void SetCapacity(unsigned int uNewCapacity);

	/// Clears the array
	/** Size will be zero but capacity remains untouched */
	inline void Clear() { m_uSize = 0; }
	/// Inserts a new element as the last one in constant time
	inline void InsertBack(T element) { operator[](m_uSize) = element; }
	// Inserts a new element at a specified position (shifting elements around)
	void InsertAt(unsigned int uIndex, T element);
	// Inserts an array of elements at the end in constant time
	void Append(const T* elements, unsigned int uAmount);
	/// Removes an element at a specified position (shifting elements around)
	void RemoveAt(unsigned int uIndex);

	// Read-only subscript operator
	const T& operator[] (const unsigned int uIndex) const;
	// Subscript operator
	T& operator[] (const unsigned int uIndex);

	/// Returns a pointer to the allocated memory
	/** It's useful when holding arrays of OpenGL data */
	inline T* const Pointer() const { return m_pArray; }
	// Allows for direct but unchecked access to the array memory
	T* MapBuffer(unsigned int uReserved);

 private:
	// Preventing copy at the moment
	ncArray(const ncArray&);
	void operator=(const ncArray&);

	T* m_pArray;
	unsigned int m_uSize;
	unsigned int m_uCapacity;
};

/// Sets a new capacity for the array (can be bigger or smaller than the current one)
template <class T>
void ncArray<T>::SetCapacity(unsigned int uNewCapacity)
{
	if (uNewCapacity == 0)
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncArray::SetCapacity - Zero is not valid capacity");
		exit(EXIT_FAILURE);
	}

	T* pNewArray = new T[uNewCapacity];

	if (m_uSize > 0)
	{
		if (uNewCapacity < m_uSize) // shrinking
		{
			m_uSize = uNewCapacity;    // cropping last elements
		}

		memcpy(pNewArray, m_pArray, sizeof(T)*m_uSize);
	}

	delete[] m_pArray;
	m_pArray = pNewArray;
	m_uCapacity = uNewCapacity;
}

/// Inserts a new element at a specified position (shifting elements around)
template <class T>
void ncArray<T>::InsertAt(unsigned int uIndex, T element)
{
	if (m_uSize + 1 > m_uCapacity)
	{
		SetCapacity(m_uSize * 2);
	}

	// memmove() takes care of overlapping regions
	memmove(m_pArray + uIndex + 1, m_pArray + uIndex, m_uSize - uIndex);
	m_pArray[uIndex] = element;
	m_uSize++;
}

/// Inserts an array of elements at the end in constant time
template <class T>
void ncArray<T>::Append(const T* elements, unsigned int uAmount)
{
	if (m_uSize + uAmount > m_uCapacity)
	{
		SetCapacity(m_uSize + uAmount);
	}

	memcpy(m_pArray + m_uSize, elements, sizeof(T)*uAmount);
	m_uSize += uAmount;
}

template <class T>
void ncArray<T>::RemoveAt(unsigned int uIndex)
{
	// memmove() takes care of overlapping regions
	memmove(m_pArray + uIndex, m_pArray + uIndex + 1, m_uSize - uIndex);
	m_uSize--;
}

/// Read-only subscript operator
template <class T>
const T& ncArray<T>::operator[] (const unsigned int uIndex) const
{
	if (uIndex > m_uSize)
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncArray::operator[] const - Element %u out of size range!", uIndex);
		exit(EXIT_FAILURE);
	}

	return m_pArray[uIndex];
}

/// Subscript operator
template <class T>
T& ncArray<T>::operator[] (const unsigned int uIndex)
{
	if (uIndex > m_uCapacity - 1)
	{
//		ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"ncArray::operator[] - Element %u out of capacity range!", uIndex);
//		exit(EXIT_FAILURE);
	}

	// Avoid creating "holes" into the array
	if (uIndex > m_uSize)
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncArray::operator[] - Element %u out of size range!", uIndex);
		exit(EXIT_FAILURE);
	}
	// Adding an element at the back of the array
	else if (uIndex == m_uSize)
	{
		// Need growing
		if (m_uSize == m_uCapacity)
		{
			SetCapacity(m_uCapacity * 2);
		}

		m_uSize++;
	}

	return m_pArray[uIndex];
}

/// Allows for direct but unchecked access to the array memory
template <class T>
T* ncArray<T>::MapBuffer(unsigned int uReserved)
{
	if (m_uSize + uReserved > m_uCapacity)
	{
		SetCapacity(m_uSize + uReserved);
	}

	T* pArray = &m_pArray[m_uSize];
	m_uSize += uReserved;

	return pArray;
}

#endif
