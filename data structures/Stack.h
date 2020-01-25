#ifndef STACK_H
#define STACK_H

#include "Vector.h"
#include "../utility/Utility.hpp"

template <typename T, template <typename> class Container = Vector>
class Stack
{
public:
	bool Empty() { return mContainer.Empty(); }
	size_t Size() { return mContainer.Size(); }

	template <typename U>
	void Push(U &&element) { mContainer.InsertLast(utility::forward<U>(element)); }
	void Pop() { mContainer.RemoveLast(); }

	T &Top() { return const_cast<T&>(static_cast<Stack const &>(*this).Top()); }
	const T &Top() const { return mContainer.Last(); }
private:
	Container<T> mContainer;
};

#endif  // STACK_H