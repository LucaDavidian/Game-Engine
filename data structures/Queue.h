#ifndef QUEUE_H
#define QUEUE_H

#include "LinkedList.h"
#include "../utility/Utility.hpp"

template <typename T, template <typename> class Container = LinkedList>
class Queue
{
public:
	bool Empty() const { return mContainer.Empty(); }
	size_t Size() const { return mContainer.Size(); }

	template <typename U>
	void Enqueue(U &&element);
	void Dequeue();

	T &Front() { return const_cast<T&>(static_cast<Queue const &>(*this).Front()); }
	const T &Front() const { return mContainer.First(); }
	T &Back() { return const_cast<T&>(static_cast<Queue const &>(*this).Back()); }
	const T &Back() const { return mContainer.Last(); }
private:
	Container<T> mContainer;
};

template <typename T, template <typename> class Container>
template <typename U>
void Queue<T,Container>::Enqueue(U &&element)
{
	mContainer.InsertLast(utility::forward<U>(element));
}

template <typename T, template <typename> class Container>
void Queue<T,Container>::Dequeue()
{
	mContainer.RemoveFirst();
}

#endif  // QUEUE_H