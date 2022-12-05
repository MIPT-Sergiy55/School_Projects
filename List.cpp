//
// Created by sergiy on 05.12.22.
//
#ifndef LIST_STACKALLOCATOR_CPP
#define LIST_STACKALLOCATOR_CPP

#include <iostream>

template<size_t N>
class StackStorage {
private:
    char* begin;
    char data[N];
public:
    StackStorage() : begin(data)
    {}

    void* alloc(size_t align, size_t size)
    {
        if (size_t(begin) % align)
            begin -= size_t(begin) % align + align;
        begin += size;
        return &begin[-size];
    }
};


template<typename T, size_t N>
class StackAllocator {
private:
    StackStorage<N>* storage;

public:
    typedef T value_type;

    template<typename U, size_t M>
    friend
    class StackAllocator;

    template<typename U>
    struct rebind {
        typedef StackAllocator<U, N> other;
    };

    StackAllocator(const StackAllocator& allocator) = default;

    template<typename U>
    StackAllocator(const StackAllocator<U, N>& allocator) : storage(allocator.storage)
    {}

    StackAllocator(StackStorage<N>& stack_storage) : storage(&stack_storage)
    {}

    T* allocate(const size_t size) const
    {
        return reinterpret_cast<T*>(storage->alloc(alignof(T), size * sizeof(T)));
    }

    void deallocate(T*, size_t) const
    {

    }

    bool operator==(const StackAllocator& allocator) const
    {
        return storage == allocator.storage;
    }

    bool operator!=(const StackAllocator& allocator) const
    {
        return storage != allocator.storage;
    }
};


template<typename T, typename Allocator = std::allocator <T>>
class List {
private:
    struct Node {
        T value;
        Node* prev = nullptr;
        Node* next = nullptr;

        explicit Node(const T& val) : value(val)
        {}

        Node() : value()
        {}
    };


    typedef typename Allocator::template rebind<Node>::other allocator;
    typedef std::allocator_traits <allocator> traits;
    typedef std::allocator_traits <Allocator> Traits;

    size_t len = 0;
    allocator alloc;
    Node* fake_node;

public:
    template<bool isConst>
    struct Iterator {
        friend class Iterator<!isConst>;

        friend class List<T, Allocator>;

    private:
        Node* node;
    public:
        typedef std::conditional_t<isConst, const T, T> value_type;
        typedef std::conditional_t<isConst, const T*, T*> pointer;
        typedef std::conditional_t<isConst, const T&, T&> reference;
        typedef std::iterator_traits <Iterator<isConst>> iterator_traits;
        typedef int difference_type;
        typedef std::bidirectional_iterator_tag iterator_category;

        Iterator() = default;

        Iterator(Node* node) : node(node)
        {}

        Iterator(const Iterator<false>& it) : node(it.node)
        {}

        Iterator& operator=(const Iterator<false>& it)
        {
            node = it.node;
            return *this;
        }

        Iterator& operator++()
        {
            node = node->next;
            return *this;
        }

        Iterator& operator--()
        {
            node = node->prev;
            return *this;
        }

        Iterator operator++(int)
        {
            Iterator copy = *this;
            ++(*this);
            return copy;
        }

        Iterator operator--(int)
        {
            Iterator copy = *this;
            --(*this);
            return copy;
        }

        Iterator& operator+=(int diff)
        {
            if (diff < 0)
                while (diff)
                {
                    --(*this);
                    ++diff;
                }
            if (diff > 0)
                while (diff)
                {
                    ++(*this);
                    --diff;
                }
            return *this;
        }

        Iterator& operator-=(int diff)
        {
            return (*this += -diff);
        }

        Iterator operator+(int diff) const
        {
            Iterator copy = *this;
            return (copy += diff);
        }

        Iterator operator-(int diff) const
        {
            return *this + (-diff);
        }

        bool operator==(const Iterator& it) const
        {
            return node == it.node;
        }

        bool operator!=(const Iterator& it) const
        {
            return node != it.node;
        }

        reference operator*() const
        {
            return node->value;
        }

        pointer operator->() const
        {
            return &(node->value);
        }
    };

    typedef Iterator<false> iterator;
    typedef Iterator<true> const_iterator;
    typedef std::reverse_iterator <Iterator<false>> reverse_iterator;
    typedef std::reverse_iterator <Iterator<true>> const_reverse_iterator;

    iterator begin() const
    {
        return iterator(fake_node->next);
    }

    iterator end() const
    {
        return iterator(fake_node);
    }

    const_iterator cbegin() const
    {
        return const_iterator(fake_node->next);
    }

    const_iterator cend() const
    {
        return const_iterator(fake_node);
    }

    reverse_iterator rbegin() const
    {
        return reverse_iterator((fake_node));
    }

    reverse_iterator rend() const
    {
        return reverse_iterator(fake_node);
    }

    const_reverse_iterator crbegin() const
    {
        return const_reverse_iterator((fake_node));
    }

    const_reverse_iterator crend() const
    {
        return const_reverse_iterator(fake_node);
    }

    List() : alloc(Allocator()), fake_node(traits::allocate(alloc, 1))
    {
        fake_node->next = fake_node;
        fake_node->prev = fake_node;
    }

    List(allocator other_alloc) : alloc(Traits::select_on_container_copy_construction(other_alloc)),
                                  fake_node(traits::allocate(alloc, 1))
    {
        fake_node->next = fake_node;
        fake_node->prev = fake_node;
    }

    List(size_t sz, Allocator other_alloc = Allocator()) : List(other_alloc)
    {
        for (size_t i = 0; i < sz; ++i)
            push_back();
    }

    List(size_t sz, const T& value, Allocator other_alloc = Allocator()) : List(other_alloc)
    {
        for (size_t i = 0; i < sz; ++i)
            push_back(value);
    }

    List(const List& list) : List(Traits::select_on_container_copy_construction(list.alloc))
    {
        for (auto& element: list)
            push_back(element);
    }

    ~List()
    {
        if (fake_node->next == nullptr)
            return;
        while (len)
            pop_back();
        traits::deallocate(alloc, fake_node, 1);
    }

    List& operator=(const List& list)
    {
        size_t old_len = len;
        if (Traits::propagate_on_container_copy_assignment::value)
            alloc = list.alloc;
        else
            alloc = Traits::select_on_container_copy_construction(list.alloc);

        try
        {
            for (auto& element: list)
                push_back(element);
        } catch (...)
        {
            while (len > old_len)
                pop_back();
        }
        while (old_len)
        {
            pop_front();
            --old_len;
        }
        return *this;
    }

    size_t size() const
    {
        return len;
    }

    void pop_back()
    {
        erase(--cend());
    }

    void pop_front()
    {
        erase(cbegin());
    }

    void push_front(const T& value)
    {
        insert(cbegin(), value);
    }

    void push_back(const T& value)
    {
        insert(cend(), value);
    }

    void insert(const_iterator it, const T& value)
    {
        Node* node = allocate_node(value);
        Node* old = it.node;
        node->next = old;
        node->prev = old->prev;
        old->prev = node;
        node->prev->next = node;
        ++len;
    }

    void erase(const_iterator it)
    {
        Node* old = it.node;
        old->prev->next = old->next;
        old->next->prev = old->prev;
        deallocate_node(old);
        --len;
    }

    allocator get_allocator() const
    {
        return alloc;
    }

private:
    void push_back()
    {
        Node* node = allocate_node();
        node->prev = fake_node->prev;
        node->next = fake_node;
        fake_node->prev->next = node;
        fake_node->prev = node;
        ++len;
    }

    Node* allocate_node(const T& value)
    {
        Node* node = traits::allocate(alloc, 1);
        try
        {
            traits::construct(alloc, node, value);
        } catch (...)
        {
            traits::deallocate(alloc, node, 1);
            throw;
        }
        return node;
    }

    Node* allocate_node()
    {
        Node* node = traits::allocate(alloc, 1);
        try
        {
            traits::construct(alloc, node);
        } catch (...)
        {
            traits::deallocate(alloc, node, 1);
            throw;
        }
        return node;
    }

    void deallocate_node(Node* node)
    {
        traits::destroy(alloc, node);
        traits::deallocate(alloc, node, 1);
    }

public:
    void print() const
    {
        Node* cur = (fake_node->next);
        while (cur != fake_node)
        {
            std::cout << cur->value << " ";
            cur = (cur->next);
        }
        std::cout << '\n';
    }
};


#endif //LIST_STACKALLOCATOR_CPP
