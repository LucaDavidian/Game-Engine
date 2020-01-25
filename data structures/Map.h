#ifndef MAP_H
#define MAP_H

#include "Vector.h"
#include "LinkedList.h"
#include "../type erasure/Function.h"
#include "../utility/Utility.hpp"
#include <exception>
#include <cstddef>
#include <string>

using std::size_t;

class ElementNotPresentException : std::exception {};

template <typename T>
size_t Hash(const T &key, size_t);

template <typename K, typename V>
class Entry
{
public:
	Entry(const K &key, const V &value) : mKey(key), mValue(value) {}
	K &Key() { return mKey; }
	V &Value() { return mValue; }
	operator Entry<const K, V>() { return Entry<const K, V>(mKey, mValue); }
private:
	K mKey;
	V mValue;
};

template <typename K, typename V>
class Map
{
private:
	typedef Entry<const K, V> Entry;
	typedef LinkedList<Entry> Bucket;
	typedef Vector<Bucket> BucketArray;
	using BucketArrayIterator = typename BucketArray::Iterator;
	using BucketIterator = typename Bucket::Iterator;
	typedef Function<size_t(const K &, size_t)> HashFunction;
public:
	class Iterator
	{
	friend class Map;
	public:
		Iterator(BucketArray *bucketArray, BucketArrayIterator bucketArrayIterator, BucketIterator bucketIterator);

		Entry &operator*() { return *mBucketIterator; }
		Entry *operator->() { return &operator*(); }
		Iterator operator++();
		Iterator operator++(int) { Iterator temp(*this); ++*this; return temp; }
		bool operator==(const Iterator &other);
		bool operator!=(const Iterator &other) { return !operator==(other); }
	private:
		BucketArray *mBucketArray;
		BucketArrayIterator mBucketArrayIterator;
		BucketIterator mBucketIterator;
	};
public:
	Map(size_t size = 97, const HashFunction & = &Hash<K>);

	bool Empty() const { return mNumElements == 0; }
	size_t Size() const { return mNumElements; }

	template <typename FwdK, typename FwdV>
	Iterator Insert(FwdK &&, FwdV &&);
	template <typename E>
	Iterator Insert(E &&);

	Iterator Remove(const K &);
	Iterator Remove(const Iterator &);

	Iterator Find(const K &);

	Iterator begin();
	Iterator end();
private:
	BucketArray mBucketArray;
	size_t mSize;
	size_t mNumElements;

	HashFunction mHashFunction;
};

/**** map's iterator implementation ****/

template <typename K, typename V>
Map<K, V>::Iterator::Iterator(BucketArray *bucketArray, BucketArrayIterator bucketArrayIterator, BucketIterator bucketIterator)
	: mBucketArray(bucketArray), mBucketArrayIterator(bucketArrayIterator), mBucketIterator(bucketIterator)
{
}

template <typename K, typename V>
typename Map<K, V>::Iterator Map<K, V>::Iterator::operator++()
{
	++mBucketIterator;

	if (mBucketIterator == mBucketArrayIterator->end())
	{
		do
		{
			++mBucketArrayIterator;
		} while (mBucketArrayIterator != mBucketArray->end() && mBucketArrayIterator->Empty());

		if (mBucketArrayIterator != mBucketArray->end())
			mBucketIterator = mBucketArrayIterator->begin();
	}

	return *this;
}

template <typename K, typename V>
bool Map<K, V>::Iterator::operator==(const Iterator &other)
{
	if (mBucketArray != other.mBucketArray || mBucketArrayIterator != other.mBucketArrayIterator)
		return false;

	if (mBucketArrayIterator == mBucketArray->end())
		return true;

	return mBucketIterator == other.mBucketIterator;
}

/**** map implementation ****/

template <typename K, typename V>
Map<K, V>::Map(size_t size, const HashFunction &hashFunction)
	: mNumElements(0), mSize(size), mHashFunction(hashFunction)
{
	for (size_t i = 0; i < mSize; i++)
		mBucketArray.InsertLast(Bucket());
}

template <typename K, typename V>
template <typename FwdK, typename FwdV>
typename Map<K, V>::Iterator Map<K, V>::Insert(FwdK &&key, FwdV &&value)
{
	Iterator it = Find(key);

	mNumElements++;

	if (it != end())
	{
		it->Value() = utility::forward<FwdV>(value);

		return it;
	}
	else
	{
		size_t index = mHashFunction(key, mSize);

		BucketArrayIterator bucketArrayIterator = mBucketArray.begin() + index;
		BucketIterator bucketIterator = bucketArrayIterator->IT_InsertLast(Entry(utility::forward<FwdK>(key), utility::forward<FwdV>(value)));

		return Iterator(&mBucketArray, bucketArrayIterator, bucketIterator);
	}
}

template <typename K, typename V>
template <typename E>
typename Map<K, V>::Iterator Map<K, V>::Insert(E &&entry)
{
	Iterator it = Find(entry.Key());

	mNumElements++;

	if (it != end())
	{
		//it->Value() = utility::forward< E >(entry.Value());  ???
		it->Value() = entry.Value();

		return it;
	}
	else
	{
		size_t index = mHashFunction(entry.Key(), mSize);

		BucketArrayIterator bucketArrayIterator = mBucketArray.begin() + index;
		BucketIterator bucketIterator = bucketArrayIterator->IT_InsertLast(utility::forward<E>(entry));

		return Iterator(&mBucketArray, bucketArrayIterator, bucketIterator);
	}
}

template <typename K, typename V>
typename Map<K, V>::Iterator Map<K, V>::Remove(const K &key)
{
	size_t index = mHashFunction(key, mSize);

	BucketArrayIterator bucketArrayIterator = mBucketArray.begin() + index;
	BucketIterator bucketIterator = bucketArrayIterator->begin();

	while (bucketIterator != bucketArrayIterator->end())
	{
		if (key == bucketIterator->Key())
		{
			bucketArrayIterator->IT_Remove(bucketIterator);

			mNumElements--;

			return Iterator(&mBucketArray, bucketArrayIterator, bucketIterator);
		}

		++bucketIterator;
	}

	throw ElementNotPresentException();
}

template <typename K, typename V>
typename Map<K, V>::Iterator Map<K, V>::Remove(const Iterator &iterator)
{
	BucketIterator bucketIterator = iterator.mBucketArrayIterator->IT_Remove(iterator.mBucketIterator);

	mNumElements--;

	return Iterator(iterator.mBucketArray, iterator.mBucketArrayIterator, bucketIterator);
}

template <typename K, typename V>
typename Map<K, V>::Iterator Map<K, V>::Find(const K &key)
{
	size_t index = mHashFunction(key, mSize);

	BucketArrayIterator bucketArrayIterator = mBucketArray.begin() + index;
	BucketIterator bucketIterator = bucketArrayIterator->begin();

	while (bucketIterator != bucketArrayIterator->end())
	{
		if (key == bucketIterator->Key())
			return Iterator(&mBucketArray, bucketArrayIterator, bucketIterator);

		++bucketIterator;
	}

	return end();
}

template <typename K, typename V>
typename Map<K, V>::Iterator Map<K, V>::begin()
{
	BucketArrayIterator bucketArrayIterator = mBucketArray.begin();
	BucketIterator bucketIterator(nullptr, nullptr);

	while (bucketArrayIterator != mBucketArray.end())
	{
		if (!bucketArrayIterator->Empty())
		{
			bucketIterator = bucketArrayIterator->begin();
			break;
		}

		++bucketArrayIterator;
	}

	return Iterator(&mBucketArray, bucketArrayIterator, bucketIterator);
}

template <typename K, typename V>
typename Map<K, V>::Iterator Map<K, V>::end()
{
	return Iterator(&mBucketArray, mBucketArray.end(), BucketIterator(nullptr, nullptr));
}

#endif  // MAP_H
