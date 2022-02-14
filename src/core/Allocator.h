#pragma once
#include "MemPool.h"

template <typename __type>
class Allocator
{
public:
    typedef  __type            value_type;
    typedef  value_type* pointer;
    typedef  value_type& reference;
    typedef  const value_type* const_pointer;
    typedef  const value_type& const_reference;
    typedef  size_t            size_type;
    typedef  ptrdiff_t         difference_type;

    Allocator(void) { }

    Allocator(Allocator const&) { }

    template <typename __other>
    Allocator(Allocator<__other> const&) { }

    // assignment.
    template <typename __other>
    Allocator& operator = (Allocator<__other> const&)
    {
        return (*this);
    }

    // rebind structure.
    template <typename __other>
    struct rebind
    {
        typedef Allocator<__other> other;
    };

    // address.
    pointer address(reference x) const
    {
        return &x;
    }

    // address.
    const_pointer address(const_reference x) const
    {
        return &x;
    }

    // allocate array of size elements.
    pointer allocate(size_type size)
    {
        return (pointer)Util::Singleton<MemPool>::getInstance()->create(size * sizeof(value_type));
    }

    // allocate array of size elements, ignore hint.
    pointer allocate(size_type size, void const*)
    {
        return allocate(size);
    }

    // deallocate object.
    void deallocate(pointer ptr, size_type size)
    {
        Util::Singleton<MemPool>::getInstance()->destroy(ptr, size * sizeof(value_type));
    }

    // default construct.
    void construct(pointer ptr) const
    {
        ::new ((void*)ptr) value_type();
    }

    // construct.
    void construct(pointer ptr, const_reference val) const
    {
        ::new ((void*)ptr) value_type(val);
    }

    // destroy.
    void destroy(pointer ptr) const
    {
        ptr->~value_type();
    }

    // estimate maximum array size
    size_type max_size(void) const
    {
        return (size_type(-1) / sizeof(value_type));
    }
};

template <typename T>
using Vector = std::vector<T, Allocator<T> >;