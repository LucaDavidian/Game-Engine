#ifndef DELEGATE_H
#define DELEGATE_H

#include "callable.hpp"
#include "connection.hpp"
#include <type_traits>
#include <exception>

/**** single-cast delegate typedefs ****/
#define DELEGATE(delegateName)                         typedef Delegate<void()> delegateName
#define DELEGATE_ONE_PARAM(delegateName, par0)         typedef Delegate<void(par0)> delegateName
#define DELEGATE_TWO_PARAM(delegateName, par0, par1)   typedef Delegate<void(par0, par1)> delegateName

#define DELEGATE_RET(delegateName, ret)                         typedef Delegate<ret()> delegateName
#define DELEGATE_RET_ONE_PARAM(delegateName, ret, par0)         typedef Delegate<ret(par0)> delegateName
#define DELEGATE_RET_TWO_PARAM(delegateName, ret, par0, par1)   typedef Delegate<ret(par0, par1)> delegateName

/**** multicast delegate typedefs ****/
#define MULTICAST_DELEGATE(delegateName)                         typedef MulticastDelegate<void()> delegateName
#define MULTICAST_DELEGATE_ONE_PARAM(delegateName, par0)         typedef MulticastDelegate<void(par0)> delegateName
#define MULTICAST_DELEGATE_TWO_PARAM(delegateName, par0, par1)   typedef MulticastDelegate<void(par0, par1)> delegateName

#define MULTICAST_DELEGATE_RET(delegateName, ret)                         typedef MulticastDelegate<ret()> delegateName
#define MULTICAST_DELEGATE_RET_ONE_PARAM(delegateName, ret, par0)         typedef MulticastDelegate<ret(par0)> delegateName
#define MULTICAST_DELEGATE_RET_TWO_PARAM(delegateName, ret, par0, par1)   typedef MulticastDelegate<ret(par0, par1)> delegateName

class AlreadyBoundException : public std::exception {};
class NotBoundException : public std::exception {};
class CallableMismatchException : public std::exception {};

/******* single-cast delegate ********/

/**** primary template ****/
template <typename Signature>
class Delegate;

/**** partial class template specialization for function types ****/
template <typename ReturnType, typename... Args>
class Delegate<ReturnType(Args...)>
{
private:
    template <typename T>
    using MemFunPtr = ReturnType (T::*)(Args...);
    template <typename T>
    using ConstMemFunPtr = ReturnType (T::*)(Args...) const;
public:
    Delegate () : mCallable(nullptr) {}
    template <typename T>
    Delegate(T&, MemFunPtr<T>);
    template <typename T>
    Delegate(const T&, ConstMemFunPtr<T>);

    ~Delegate() { delete mCallable; }

    template <typename T>     
    void Bind(T&, MemFunPtr<T>);
    template <typename T>     
    void Bind(const T&, ConstMemFunPtr<T>);

    void Unbind(CallableBase<ReturnType(Args...)>*);

    ReturnType Invoke(Args...);

    bool IsBound() const;
private:
    CallableBase<ReturnType(Args...)> *mCallable;
};

template <typename ReturnType, typename... Args>
template <typename T>
Delegate<ReturnType(Args...)>::Delegate(T &instance, MemFunPtr<T> memFunPtr) : mCallable(new Callable<T,ReturnType(Args...)>(instance, memFunPtr)) 
{
    instance.Connect(new Connection<ReturnType(Args...)>(this, mCallable));
}

template <typename ReturnType, typename... Args>
template <typename T>
Delegate<ReturnType(Args...)>::Delegate(const T &instance, ConstMemFunPtr<T> constMemFunPtr) : mCallable(new ConstCallable<T,ReturnType(Args...)>(instance, constMemFunPtr)) 
{
    instance.Connect(new Connection<ReturnType(Args...)>(this, mCallable));
}

template <typename ReturnType, typename... Args>
template <typename T>
void Delegate<ReturnType(Args...)>::Bind(T &instance, MemFunPtr<T> memFunPtr)
{
    // if delegate is already bound an exception is raised (cant' notify delegating object)
    if (IsBound())
        throw AlreadyBoundException();
    
    mCallable = new Callable<T,ReturnType(Args...)>(instance, memFunPtr);
    instance.Connect(new Connection<ReturnType(Args...)>(this, mCallable));
}

template <typename ReturnType, typename... Args>
template <typename T>     
void Delegate<ReturnType(Args...)>::Bind(const T &instance, ConstMemFunPtr<T> constMemFunPtr)
{
    // if delegate is already bound an exception is raised (cant' notify delegating object)
    if (IsBound())
        throw AlreadyBoundException();

    mCallable = new ConstCallable<T,ReturnType(Args...)>(instance, constMemFunPtr);
    instance.Connect(new Connection<ReturnType(Args...)>(this, mCallable));
}

template <typename ReturnType, typename... Args>
void Delegate<ReturnType(Args...)>::Unbind(CallableBase<ReturnType(Args...)> *callable)
{
    // if trying to unbind another callable an exception is raised
    if (callable != mCallable)
        throw CallableMismatchException();

    delete mCallable;
    mCallable = nullptr;
}

template <typename ReturnType, typename... Args>
ReturnType Delegate<ReturnType(Args...)>::Invoke(Args... args)
{
    // if delegate is not bound and it's invoked an exception is raised
    if (!IsBound())
        throw NotBoundException();

    return mCallable->Invoke(args...);
}

template <typename ReturnType, typename... Args>
bool Delegate<ReturnType(Args...)>::IsBound() const
{
    return mCallable != nullptr;
}

/******** multicast delegate ********/

#include <vector>

/**** primary template ****/
template <typename Signature>
class MulticastDelegate;

/**** partial class template specialization for function types ****/
template <typename ReturnType, typename... Args>
class MulticastDelegate<ReturnType(Args...)>
{
private:
    template <typename T>
    using MemFunPtr = ReturnType (T::*)(Args...);
    template <typename T>
    using ConstMemFunPtr = ReturnType (T::*)(Args...) const; 
public:
    MulticastDelegate() = default;    
    ~MulticastDelegate() { for (CallableBase<ReturnType(Args...)> *callable : mCallables) delete callable; }

    template <typename T>
    void Bind(T&, MemFunPtr<T>);
    template <typename T>
    void Bind(T const &, ConstMemFunPtr<T>);

    void Unbind(CallableBase<ReturnType(Args...)>*);

    ReturnType Invoke(Args... args) const { for (CallableBase<ReturnType(Args...)> *callable : mCallables) callable->Invoke(args...); }  
private:
    std::vector<CallableBase<ReturnType(Args...)>*> mCallables;
};

template <typename ReturnType, typename... Args>
template <typename T>
void MulticastDelegate<ReturnType(Args...)>::Bind(T &instance, MemFunPtr<T> memFunPtr) 
{ 
    Callable<T,ReturnType(Args...)> *callable = new Callable<T, ReturnType(Args...)>(instance, memFunPtr);
    mCallables.push_back(callable); 
    instance.Connect(new MulticastConnection<ReturnType(Args...)>(this, callable));
}
    
template <typename ReturnType, typename... Args>
template <typename T>
void MulticastDelegate<ReturnType(Args...)>::Bind(const T &instance, ConstMemFunPtr<T> constMemFunPtr) 
{ 
    ConstCallable<T,ReturnType(Args...)> *callable = new ConstCallable<T, ReturnType(Args...)>(instance, constMemFunPtr);
    mCallables.push_back(callable); 
    instance.Connect(new MulticastConnection<ReturnType(Args...)>(this, callable));
}

template <typename ReturnType, typename... Args>
void MulticastDelegate<ReturnType(Args...)>::Unbind(CallableBase<ReturnType(Args...)> *callable)
{
    typename std::vector<CallableBase<ReturnType(Args...)>*>::const_iterator cit = mCallables.begin();

    while (cit != mCallables.end())
    {
        if (*cit == callable)
        {
            delete *cit;
            mCallables.erase(cit);
            return;
        }

        ++cit;
    }
}

#endif  // DELEGATE_H