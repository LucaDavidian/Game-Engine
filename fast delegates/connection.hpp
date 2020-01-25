#ifndef CONNECTION_H
#define CONNECTION_H

template <typename Signature>
class Delegate;

template <typename Singature>
class MulticastDelegate;

/**** Connection abstract base class ****/
class ConnectionBase
{
public:
    virtual ~ConnectionBase() = default;
    virtual void Disconnect() = 0;
protected:
    ConnectionBase() = default;
};

/**** Connection - primary class template ****/
template <typename Signature>
class Connection;

/**** Connection - partial class template specialization for function types ****/
template <typename ReturnType, typename... Args>
class Connection<ReturnType(Args...)> : public ConnectionBase
{
public:
    Connection(Delegate<ReturnType(Args...)> *delegate, typename Delegate<ReturnType(Args...)>::Callable *callable) : mDelegate(delegate), mCallable(callable) {}
    void Disconnect() override { mDelegate->Unbind(mCallable); }
private:    
    Delegate<ReturnType(Args...)> *mDelegate;
    typename Delegate<ReturnType(Args...)>::Callable *mCallable;
};

/**** MulticastConnection - primary class template ****/
template <typename Signature>
class MulticastConnection;

/**** MulticastConnection - partial class template specialization for function types ****/
template <typename ReturnType, typename... Args>
class MulticastConnection<ReturnType(Args...)> : public ConnectionBase
{
public:
    MulticastConnection(MulticastDelegate<ReturnType(Args...)> *delegate, typename MulticastDelegate<ReturnType(Args...)>::Callable *callable) : mDelegate(delegate), mCallable(callable) {}
    void Disconnect() override { mDelegate->Unbind(mCallable); }
private:    
    MulticastDelegate<ReturnType(Args...)> *mDelegate;
    typename MulticastDelegate<ReturnType(Args...)>::Callable *mCallable;
};

#endif  // CONNECTION_H