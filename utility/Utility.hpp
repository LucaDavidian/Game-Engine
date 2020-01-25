#ifndef MOVE_FORWARD_H
#define MOVE_FORWARD_H

#include "../utility/Traits.hpp"

/**** r-value references, move semantics and forwarding (universal) references ****/

namespace utility
{
    // move
    template <typename T>
    typename traits::remove_reference<T>::type &&move(T &&t)
    {
        using return_type = traits::remove_reference_t<T>&&;
        return static_cast<return_type>(t);
    }

    // forward
    template <typename T>
    T &&forward(traits::remove_reference_t<T> &t)
    {
        return static_cast<T&&>(t);
    }
}

#endif    // MOVE_FORWARD_H