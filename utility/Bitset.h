#ifndef BITSET_H
#define BITSET_H

/**** fixed length bitmask implementation ****/

#include <cstddef>
#include <limits>
#include <iostream>

using std::size_t;

template <unsigned int N>
class Bitset;

template <unsigned int N>
std::ostream &operator<<(std::ostream &os, const Bitset<N> &bitset);

/**** Bitset and BitProxy definition ****/
template <unsigned int N>
class Bitset
{
friend std::ostream &operator<<<N>(std::ostream &, const Bitset &);
private:
	class BitProxy
	{
	public:
		BitProxy(unsigned int *bitmask, size_t bitPosition) : mBitmask(bitmask), mBitPosition(bitPosition) {}
		BitProxy &operator=(bool bitValue);
	private:
		unsigned int *mBitmask;
		size_t mBitPosition;
	};
public:
	Bitset();

	size_t Size() const { return N; }

	BitProxy operator[](size_t index);
	bool operator[](size_t index) const;

	bool operator==(const Bitset &other) const;
	bool operator!=(const Bitset &other) const;

	Bitset operator~() const { Bitset temp(*this); temp.Flip(); return temp; }

	Bitset operator&(const Bitset &other) const;
	Bitset operator^(const Bitset &other) const;
	Bitset operator|(const Bitset &other) const;

	Bitset &operator&=(const Bitset &other) { *this = *this & other; return *this; }
	Bitset &operator^=(const Bitset &other) { *this = *this ^ other; return *this; }
	Bitset &operator|=(const Bitset &other) { *this = *this | other; return *this; }

	void Set(size_t index);
	void Set();
	void Reset(size_t index);
	void Reset();
	void Flip(size_t index);
	void Flip();
	bool Test(size_t index) const;
private:
	static constexpr size_t SIZE = (N - 1) / std::numeric_limits<unsigned int>::digits + 1;
	unsigned int mBitmask[SIZE];
};

/**** stream output operator function definition ****/
template <unsigned int N>
std::ostream &operator<<(std::ostream &os, const Bitset<N> &bitset)
{
	int n = N - 1;

	for (size_t i = 0; i < Bitset<N>::SIZE; i++)
		for (int pos = n % std::numeric_limits<unsigned int>::digits; pos >= 0; pos--, n--)
			os << static_cast<bool>(bitset.mBitmask[i] & 1U << pos);

	// for (int pos = N - 1; pos >= 0; pos--)
	//     os << bitset[pos];  
}

/**** Bitset<N>::BitProxy member definitions ****/
template <unsigned int N>
typename Bitset<N>::BitProxy &Bitset<N>::BitProxy::operator=(bool bitValue)
{
	if (bitValue)
		*mBitmask |= 1U << mBitPosition;
	else
		*mBitmask &= ~(1U << mBitPosition);
}

/**** Bitset<N> member definitions ****/
template <unsigned int N>
Bitset<N>::Bitset()
{
	for (int i = 0; i < SIZE; i++)
		mBitmask[i] = 0U;
}

template <unsigned int N>
typename Bitset<N>::BitProxy Bitset<N>::operator[](size_t index)
{
	unsigned int *bitmask = &mBitmask[SIZE - 1 - index / std::numeric_limits<unsigned int>::digits];
	size_t bitPosition = index % std::numeric_limits<unsigned int>::digits;

	return BitProxy(bitmask, bitPosition);
}

template <unsigned int N>
bool Bitset<N>::operator[](size_t index) const
{
	unsigned int bitmask = mBitmask[SIZE - 1 - index / std::numeric_limits<unsigned int>::digits];
	size_t bitPosition = index % std::numeric_limits<unsigned int>::digits;

	return bitmask & 1U << bitPosition;
}

template <unsigned int N>
bool Bitset<N>::operator==(const Bitset &other) const
{
	for (size_t i = 0; i < SIZE; i++)
		if (mBitmask[i] != other.mBitmask[i])
			return false;

	return true;
}

template <unsigned int N>
bool Bitset<N>::operator!=(const Bitset &other) const
{
	return !(*this == other);
}

template <unsigned int N>
Bitset<N> Bitset<N>::operator&(const Bitset &other) const
{
	Bitset temp(*this);

	for (size_t i = 0; i < SIZE; i++)
		temp.mBitmask[i] &= other.mBitmask[i];

	return temp;
}

template <unsigned int N>
Bitset<N> Bitset<N>::operator^(const Bitset &other) const
{
	Bitset temp(*this);

	for (size_t i = 0; i < SIZE; i++)
		temp.mBitmask[i] ^= other.mBitmask[i];

	return temp;
}

template <unsigned int N>
Bitset<N> Bitset<N>::operator|(const Bitset &other) const
{
	Bitset temp(*this);

	for (size_t i = 0; i < SIZE; i++)
		temp.mBitmask[i] |= other.mBitmask[i];

	return temp;
}

template <unsigned int N>
void Bitset<N>::Set(size_t index)
{
	mBitmask[SIZE - 1 - index / std::numeric_limits<unsigned int>::digits] |= 1U << index % std::numeric_limits<unsigned int>::digits;
}

template <unsigned int N>
void Bitset<N>::Set()
{
	int n = N - 1;

	for (size_t i = 0; i < SIZE; i++)
		for (int pos = n % std::numeric_limits<unsigned int>::digits; pos >= 0; pos--, n--)
			mBitmask[i] |= 1U << pos;
}

template <unsigned int N>
void Bitset<N>::Reset(size_t index)
{
	mBitmask[SIZE - 1 - index / std::numeric_limits<unsigned int>::digits] &= ~(1U << index % std::numeric_limits<unsigned int>::digits);
}

template <unsigned int N>
void Bitset<N>::Reset()
{
	int n = N - 1;

	for (size_t i = 0; i < SIZE; i++)
		for (int pos = n % std::numeric_limits<unsigned int>::digits; pos >= 0; pos--, n--)
			mBitmask[i] &= ~(1U << pos);
}

template <unsigned int N>
void Bitset<N>::Flip(size_t index)
{
	mBitmask[SIZE - 1 - index / std::numeric_limits<unsigned int>::digits] ^= 1U << index % std::numeric_limits<unsigned int>::digits;
}

template <unsigned int N>
void Bitset<N>::Flip()
{
	int n = N - 1;

	for (size_t i = 0; i < SIZE; i++)
		for (int pos = n % std::numeric_limits<unsigned int>::digits; pos >= 0; pos--, n--)
			mBitmask[i] ^= 1U << pos;
}

template <unsigned int N>
bool Bitset<N>::Test(size_t index) const
{
	return mBitmask[SIZE - 1 - index / std::numeric_limits<unsigned int>::digits] & 1U << index % std::numeric_limits<unsigned int>::digits;
}


#endif  // BITSET_H