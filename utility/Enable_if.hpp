namespace traits
{
    template <bool, typename = void>
    struct enable_if
    {
    };

    template <typename T>
    struct enable_if<true, T>  
    {
        typedef T type;
    };

    // convenience alias template (C++14)
    template <bool Condition, typename T = void>
    using enable_if_t = typename enable_if<Condition, T>::type;
}

