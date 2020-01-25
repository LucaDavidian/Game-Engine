#ifndef CONNECTION_H
#define CONNECTION_H

//#define CONNECTION(NAME)      private:                                              \  
//								 Vector<ConnectionBase*> mConnections;              \
//						      public:                                               \
//								~##NAME() { for (ConnectionBase * connection : mConnections) { connection->Disconnect(); delete connection; } }  \
//								void Connect(ConnectionBase *connection) { mConnections.InsertLast(connection); }   \

/**** a connection object represents a connection between a delegate and the delegating object ****/

template <typename Signature>
class Delegate;

template <typename Signature>
class MulticastDelegate;

template <typename Signature>
class CallableBase;

/**** connection abstract base class ****/
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
    Connection(Delegate<ReturnType(Args...)> *delegate, CallableBase<ReturnType(Args...)> *callable) : mDelegate(delegate), mCallable(callable) {}
    void Disconnect() override {mDelegate->Unbind(mCallable); }
private:
    Delegate<ReturnType(Args...)> *mDelegate;
    CallableBase<ReturnType(Args...)> *mCallable;
};

/**** MulticastConnection - primary class template ****/
template <typename Signature>
class MulticastConnection;

/**** MulticastConnection - partial class template specialization for function types ****/
template <typename ReturnType, typename... Args>
class MulticastConnection<ReturnType(Args...)> : public ConnectionBase
{
public:
    MulticastConnection(MulticastDelegate<ReturnType(Args...)> *delegate, CallableBase<ReturnType(Args...)> *callable) : mDelegate(delegate), mCallable(callable) {}
    void Disconnect() override {mDelegate->Unbind(mCallable); }
private:
    MulticastDelegate<ReturnType(Args...)> *mDelegate;
    CallableBase<ReturnType(Args...)> *mCallable;
};

#endif  // CONNECTION_H