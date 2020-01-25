#ifndef DELEGATE_H
#define DELEGATE_H

#include <exception>
#include "connection.hpp"

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

/******** single cast delegate ********/

/**** primary template ****/
template <typename Signature>
class Delegate;

/**** partial class template specialization for function types ****/
template <typename ReturnType, typename... Args>
class Delegate<ReturnType(Args...)>
{
friend class Connection<ReturnType(Args...)>;
private:
    using Instance = void*;
    using Callback = ReturnType (*)(Instance, Args...);
    struct Callable
    {
        Callable(Instance instance, Callback callback) : mInstance(instance), mCallback(callback) {}
        Instance mInstance;
        Callback mCallback;
    };
public:
    Delegate() : mCallable(nullptr) {}
    ~Delegate() { delete mCallable; }

    template <typename T, ReturnType (T::*)(Args...)>
    void Bind(T&);
    template <typename T, ReturnType (T::*)(Args...) const>
    void Bind(const T&);

    void Unbind(Callable*);

    bool IsBound() const { return mCallable; }

    ReturnType Invoke(Args...) const;
private:
    template <typename T, ReturnType (T::*)(Args...)>
    static ReturnType Stub(Instance, Args...);
    template <typename T, ReturnType (T::*)(Args...) const>
    static ReturnType Stub(Instance, Args...);

    Callable *mCallable;
};

template <typename ReturnType, typename... Args>
template <typename T, ReturnType (T::*MemFunPtr)(Args...)>
void Delegate<ReturnType(Args...)>::Bind(T &instance)
{
    if (IsBound())
        throw AlreadyBoundException();

    mCallable = new Callable{ &instance, &Stub<T,MemFunPtr> };
    instance.Connect(new Connection<ReturnType(Args...)>(this, mCallable));
}

template <typename ReturnType, typename... Args>
template <typename T, ReturnType (T::*ConstMemFunPtr)(Args...) const>
void Delegate<ReturnType(Args...)>::Bind(const T &instance)
{
    if (IsBound())
        throw AlreadyBoundException();

    mCallable = new Callable{ const_cast<T*>(&instance), &Stub<const T, ConstMemFunPtr> };
    instance.Connect(new Connection<ReturnType(Args...)>(this, mCallable));
}

template <typename ReturnType, typename... Args>
void Delegate<ReturnType(Args...)>::Unbind(Callable *callable)
{
    if (callable != mCallable)
        throw CallableMismatchException();

    delete mCallable;
    mCallable = nullptr;
}

template <typename ReturnType, typename... Args>
ReturnType Delegate<ReturnType(Args...)>::Invoke(Args... args) const
{
    if (!IsBound())
        throw NotBoundException();

    return mCallable->mCallback(mCallable->mInstance, args...);
}

template <typename ReturnType, typename... Args>
template <typename T, ReturnType (T::*MemFunPtr)(Args...)>
ReturnType Delegate<ReturnType(Args...)>::Stub(Instance instance, Args... args)
{
    return (static_cast<T*>(instance)->*MemFunPtr)(args...);
}

template <typename ReturnType, typename... Args>
template <typename T, ReturnType (T::*ConstMemFunPtr)(Args...) const>
ReturnType Delegate<ReturnType(Args...)>::Stub(Instance instance, Args... args)
{
    return (static_cast<T*>(instance)->*ConstMemFunPtr)(args...);
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
friend class MulticastConnection<ReturnType(Args...)>;
private:
    typedef void *Instance;
    typedef ReturnType (*Callback)(Instance, Args...); 
    struct Callable
    {
        Callable(Instance instance, Callback callback) : mInstance(instance), mCallback(callback) {}
        Instance mInstance;
        Callback mCallback;
    };  
    template <typename T>
    using MemFunPtr = ReturnType (T::*)(Args...);
    template <typename T>
    using ConstMemFunPtr = ReturnType (T::*)(Args...) const;
public:
    MulticastDelegate() = default;
    ~MulticastDelegate() { for (Callable *callable : mCallables) delete callable; }

    template <typename T, ReturnType (T::*)(Args...)>
    void Bind(T&);
    template <typename T, ReturnType (T::*)(Args...) const>
    void Bind(T const &);

    void Unbind(Callable*);

    void Invoke(Args...);
private:
    template <typename T, ReturnType (T::*)(Args...)>
    static void Stub(void*, Args...);
    template <typename T, ReturnType (T::*)(Args...) const>
    static void Stub(void*, Args...);

    std::vector<Callable*> mCallables;
};

template <typename ReturnType, typename... Args>
template <typename T, ReturnType (T::*MemFunPtr)(Args...)>
void MulticastDelegate<ReturnType(Args...)>::Bind(T &instance)
{
    Callable *callable =  new Callable(&instance, &Stub<T,MemFunPtr>);
   
    mCallables.push_back(callable);
    instance.Connect(new MulticastConnection<void(Args...)>(this, callable));
}

template <typename ReturnType, typename... Args>
template <typename T, ReturnType (T::*ConstMemFunPtr)(Args...) const>
void MulticastDelegate<ReturnType(Args...)>::Bind(T const &instance)
{
    Callable *callable =  new Callable(const_cast<T*>(&instance), &Stub<const T,ConstMemFunPtr>);
   
    mCallables.push_back(callable);
    instance.Connect(new MulticastConnection<void(Args...)>(this, callable));
}

template <typename ReturnType, typename... Args>
void MulticastDelegate<ReturnType(Args...)>::Unbind(Callable *callable)
{
    typename std::vector<Callable*>::const_iterator cit = mCallables.begin();

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

template <typename ReturnType, typename... Args>
void MulticastDelegate<ReturnType(Args...)>::Invoke(Args... args)
{
    for (Callable *callable: mCallables)
        callable->mCallback(callable->mInstance, args...);
}

template <typename ReturnType, typename... Args>
template <typename T, ReturnType (T::*MemFunPtr)(Args...)>
void MulticastDelegate<ReturnType(Args...)>::Stub(void *instance, Args... args)
{
    (static_cast<T*>(instance)->*MemFunPtr)(args...);
}

template <typename ReturnType, typename... Args>
template <typename T, ReturnType (T::*ConstMemFunPtr)(Args...) const>
void MulticastDelegate<ReturnType(Args...)>::Stub(void *instance, Args... args)
{
    (static_cast<T*>(instance)->*ConstMemFunPtr)(args...);
}

#endif // DELEGATE_H