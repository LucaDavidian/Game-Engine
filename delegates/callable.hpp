#ifndef CALLABLE_H
#define CALLABLE_H

// a callable is a wrapper around an object and a member function/const member function 
// only member functions are considered because an object must implement a Connect method 

/*** callable abstract base class ****/
template <typename Signature>
class CallableBase;

template <typename ReturnType, typename... Args>
class CallableBase<ReturnType(Args...)>
{
public:
    virtual ~CallableBase() = default;
    virtual ReturnType Invoke(Args...) = 0;
protected:
    CallableBase() = default;
};

/**** member function callable ****/
template <typename T, typename Signature>
class Callable;

template <typename T, typename ReturnType, typename... Args>
class Callable<T, ReturnType(Args...)> : public CallableBase<ReturnType(Args...)>
{
private:
    using PtrToMemFun = ReturnType (T::*)(Args...);
public:
    Callable(T &instance, PtrToMemFun ptrToMemFun) : mInstance(instance), mPtrToMemFun(ptrToMemFun) {}
    ReturnType Invoke(Args...) override;
private:
    T &mInstance;
    PtrToMemFun mPtrToMemFun;
};

template <typename T, typename ReturnType, typename... Args>
ReturnType Callable<T, ReturnType(Args...)>::Invoke(Args... args)
{
    return (mInstance.*mPtrToMemFun)(args...);
}

/**** const member function callable ****/
template <typename T, typename Signature>
class ConstCallable;

template <typename T, typename ReturnType, typename... Args>
class ConstCallable<T, ReturnType(Args...)> : public CallableBase<ReturnType(Args...)>
{
private:
    using PtrToConstMemFun = ReturnType (T::*)(Args...) const;
public:
    ConstCallable(const T &instance, PtrToConstMemFun ptrToConstMemFun) : mInstance(instance), mPtrToConstMemFun(ptrToConstMemFun) {}
    ReturnType Invoke(Args...) override;
private:
    const T &mInstance;
    PtrToConstMemFun mPtrToConstMemFun;
};

template <typename T, typename ReturnType, typename... Args>
ReturnType ConstCallable<T, ReturnType(Args...)>::Invoke(Args... args)
{
    return (mInstance.*mPtrToConstMemFun)(args...);
}

#endif  // CALLABLE_H