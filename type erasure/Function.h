#ifndef FUNCTION_H
#define FUNCTION_H

#include "../utility/Utility.hpp"
#include "../utility/Enable_if.hpp"
#include "../utility/Traits.hpp"

/**** primary template (declaration) ****/
template <typename Signature>
class Function;

/**** alias template for SFINAE ****/
template <typename Signature, typename T>
using IsNotSameAsFunction = traits::enable_if_t<!traits::is_same<Function<Signature>, traits::decay_t<T>>::value>;  // decay must be used for SFINAE

/**** swap function in global scope ****/
template <typename Signature>
void swap(Function<Signature> &a, Function<Signature> &b)
{
    a.Swap(b);
}

/**** partial class template specialization for function types ****/
template <typename ReturnType, typename... Args>
class Function<ReturnType(Args...)>
{
private:
    class Concept
    {
    public:
        virtual ~Concept() = default;
        virtual Concept *Clone() = 0;
        virtual ReturnType Invoke(Args...) = 0;
    protected:
        Concept() = default;
    };
    template <typename T>
    class Model : public Concept
    {
    public:
        template <typename U>
        Model(U&&);
        Model *Clone() override;
        ReturnType Invoke(Args...) override;
    private:
        T mObject;
    };
public:
    // destructor
    ~Function() { delete mConcept; }
    
    // constructors
    Function(const Function&);
    // Function(Function &other) : Function(static_cast<const Function&>(other)) {} // delegating constructor - if SFINAE is not used (inhibit forwarding constructor)
    Function(Function&&);
    template <typename T, typename = IsNotSameAsFunction<ReturnType(Args...),T>>  /* typename = traits::enable_if<!traits::is_same<T,Function>::value>::type */
    Function(T&&);

    // assignment operators
    Function &operator=(const Function&);
    // Function &operator=(Function &other) { return *this = static_cast<const Function&>(other); }  // if SFINAE is not used (inhibit forwarding assignment operator)
    Function &operator=(Function&&);
    template <typename T, typename = IsNotSameAsFunction<ReturnType(Args...),T>>  /* typename = traits::enable_if<!traits::is_same<T,Function>::value>::type */
    Function &operator=(T&&);

    // overloaded function call operator
    ReturnType operator()(Args... args) { return mConcept->Invoke(args...); }

    // swap function
    void Swap(Function&);
private:
    Concept *mConcept;
};

/**** Model class implementation ****/
template <typename ReturnType, typename... Args>
template <typename T>
template <typename U>
Function<ReturnType(Args...)>::Model<T>::Model(U &&object) : mObject(utility::forward<U>(object))
{
}

template <typename ReturnType, typename... Args>
template <typename T>
Function<ReturnType(Args...)>::Model<T> *Function<ReturnType(Args...)>::Model<T>::Clone()
{
    return new Model(mObject);
}

template <typename ReturnType, typename... Args>
template <typename T>
ReturnType Function<ReturnType(Args...)>::Model<T>::Invoke(Args... args)
{
   return mObject(args...);
}

/**** Function class implementation ****/
template <typename ReturnType, typename... Args>
Function<ReturnType(Args...)>::Function(const Function &other) : mConcept(other.mConcept->Clone())
{
}

#include <iostream>
template <typename ReturnType, typename... Args>
Function<ReturnType(Args...)>::Function(Function &&other) : mConcept(other.mConcept)
{
    other.mConcept = nullptr;
}

template <typename ReturnType, typename... Args>
template <typename T, typename>
Function<ReturnType(Args...)>::Function(T &&object) : mConcept(new Model<typename std::decay<T>::type>(utility::forward<T>(object)))
{
}

template <typename ReturnType, typename... Args>
Function<ReturnType(Args...)> &Function<ReturnType(Args...)>::operator=(const Function &other)
{
    Function temp(other);
    Swap(temp);

    return *this;
}

template <typename ReturnType, typename... Args>
Function<ReturnType(Args...)> &Function<ReturnType(Args...)>::operator=(Function &&other)
{
    Swap(other);

    return *this;
}

template <typename ReturnType, typename... Args>
template <typename T, typename>
Function<ReturnType(Args...)> &Function<ReturnType(Args...)>::operator=(T &&object)
{
    Function temp(utility::forward<T>(object));
    Swap(temp);

    return *this;
}

template <typename ReturnType, typename... Args>
void Function<ReturnType(Args...)>::Swap(Function &other)
{
    Concept *temp = mConcept;
    mConcept = other.mConcept;
    other.mConcept = temp;
}

#endif // FUNCTION_H

