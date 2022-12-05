//
// Created by sergiy on 05.12.22.
//
#include <iostream>
#include <vector>

const size_t block_size = 8;

template <typename T>
class Deque {
private:
    size_t capacity;
    size_t outer_Begin;
    size_t inner_Begin;
    size_t outer_End;
    size_t inner_End;
    std::vector<T*> data;

    void change_capacity(size_t new_capacity) {
        if (new_capacity <= capacity)
            return;
        data.resize(new_capacity, nullptr);
        size_t dif = (new_capacity - capacity) / 2;
        for (int i = outer_End; i >= int(outer_Begin); --i) {
            data[i + dif] = data[i];
        }
        for (size_t i = 0; i < outer_Begin + dif; ++i) {
            data[i] = nullptr;
        }
        capacity = new_capacity;
        outer_Begin += dif;
        outer_End += dif;
    }

    void increase_capacity() {
        if (outer_Begin == 0 || outer_End == capacity - 1)
            change_capacity(capacity == 0 ? 3 : capacity * 2);
    }

    void add_block_front() {
        increase_capacity();
        data[outer_Begin - 1] = reinterpret_cast<T*>(new uint8_t[block_size * sizeof(T)]);
    }

    void add_block_back() {
        increase_capacity();
        data[outer_End + 1] = reinterpret_cast<T*>(new uint8_t[block_size * sizeof(T)]);
    }

public:
    template<bool isConst>
    class Iterator{
        friend class Iterator<true>;
    private:
        T** ptr = nullptr;
        size_t idx;
    public:
        typedef std::conditional_t<isConst, const T, T> value_type;
        typedef std::conditional_t<isConst, const T*, T*> pointer;
        typedef std::conditional_t<isConst, const T&, T&> reference;
        typedef int difference_type;
        typedef std::random_access_iterator_tag iterator_category;

        Iterator() = default;
        Iterator(T** pointer, size_t index) {
            ptr = pointer;
            idx = index;
        }
        template<bool Const>
        Iterator(const Iterator<isConst && Const>& it) {
            ptr = it.ptr;
            idx = it.idx;
        }
        ~Iterator() = default;
        Iterator& operator=(const Iterator<false>& it) {
            ptr = it.ptr;
            idx = it.idx;
            return *this;
        }

        Iterator& operator++() {if (idx < block_size - 1)
                ++idx;
            else {
                idx = 0;
                ++ptr;
            }
            return *this;
        }
        Iterator operator++(int) {
            Iterator copy(*this);
            ++(*this);
            return copy;
        }
        Iterator& operator--() {if (idx > 0)
                --idx;
            else {
                idx = block_size - 1;
                --ptr;
            }
            return *this;
        }
        Iterator operator--(int) {
            Iterator copy(*this);
            --(*this);
            return copy;
        }

        Iterator operator+(difference_type dif) const {
            std::cerr << "+ " << dif << " ";
            Iterator copy(*this);
            if(dif > 0) {
                copy.idx = (idx + dif) % block_size;
                copy.ptr += (idx + dif) / block_size;
            } else if (dif < 0){
                copy.idx = ((int(idx) + dif) % block_size + block_size) % block_size;
                copy.ptr += ((int(idx) + dif) - int(block_size) + 1) / int(block_size);
            }
            return copy;
        }
        Iterator operator-(difference_type dif) const {
            return (*this) + (-dif);
        }

        bool operator==(const Iterator<isConst>& it) const {
            return ptr == it.ptr && idx == it.idx;
        }
        bool operator<(const Iterator<isConst>& it) const {
            if (ptr == it.ptr)
                return idx < it.idx;
            return ptr < it.ptr;
        }
        bool operator>(const Iterator<isConst>& it) const {
            return it < *this;
        }
        bool operator<=(const Iterator<isConst>& it) const {
            return *this < it || *this == it;
        }
        bool operator>=(const Iterator<isConst>& it) const {
            return it <= *this;
        }
        bool operator!=(const Iterator<isConst>& it) const {
            return !(*this == it);
        }

        difference_type operator-(const Iterator<isConst>& it) const {
            return (idx - it.idx) + (ptr - it.ptr) * block_size;
        }

        reference operator*() const {
            std::cerr << "* ";
            return *(*ptr + idx);
        }
        pointer operator->() const {
            std::cerr << "-> ";
            return *ptr + idx;
        }
    };



    template<bool isConst>
    class ReverseIterator{
        friend class ReverseIterator<true>;
    private:
        T** ptr = nullptr;
        size_t idx;
    public:
        typedef std::conditional_t<isConst, const T, T> value_type;
        typedef std::conditional_t<isConst, const T*, T*> pointer;
        typedef std::conditional_t<isConst, const T&, T&> reference;
        typedef int difference_type;
        typedef std::random_access_iterator_tag iterator_category;

        ReverseIterator() = default;
        ReverseIterator(T** pointer, size_t index) {
            ptr = pointer;
            idx = index;
        }
        template<bool Const>
        ReverseIterator(const ReverseIterator<isConst && Const>& it) {
            ptr = it.ptr;
            idx = it.idx;
        }
        ~ReverseIterator();
        ReverseIterator& operator=(const ReverseIterator<false>& it) {
            ptr = it.ptr;
            idx = it.idx;
            return *this;
        }

        ReverseIterator& operator++() {
            if (idx > 0)
                --idx;
            else {
                idx = block_size - 1;
                --ptr;
            }
            return *this;
        }
        ReverseIterator operator++(int) {
            ReverseIterator copy(*this);
            ++(*this);
            return copy;
        }
        ReverseIterator& operator--() {
            if (idx < block_size - 1)
                ++idx;
            else {
                idx = 0;
                ++ptr;
            }
            return *this;
        }
        ReverseIterator operator--(int) {
            ReverseIterator copy(*this);
            --(*this);
            return copy;
        }

        ReverseIterator operator+(difference_type dif) const {
            return (*this) - (-dif);
        }
        ReverseIterator operator-(difference_type dif) const {
            ReverseIterator copy(*this);
            if(dif > 0) {
                copy.idx = (idx + dif) % block_size;
                copy.ptr += (idx + dif) / block_size;
            } else if (dif < 0){
                copy.idx = ((int(idx) + dif) % block_size + block_size) % block_size;
                copy.ptr += ((int(idx) + dif) - int(block_size) + 1) / int(block_size);
            }
            return copy;
        }

        bool operator==(const ReverseIterator<isConst>& it) const {
            return ptr == it.ptr && idx == it.idx;
        }
        bool operator>(const ReverseIterator<isConst>& it) const {
            if (ptr == it.ptr)
                return idx < it.idx;
            return ptr < it.ptr;
        }
        bool operator<(const ReverseIterator<isConst>& it) const {
            return it < *this;
        }
        bool operator<=(const ReverseIterator<isConst>& it) const {
            return *this < it || *this == it;
        }
        bool operator>=(const ReverseIterator<isConst>& it) const {
            return it <= *this;
        }
        bool operator!=(const ReverseIterator<isConst>& it) const {
            return !(it == *this);
        }

        difference_type operator-(const ReverseIterator<isConst>& it) const {
            std::cerr << "rdif ";
            return (it.idx - idx) + (it.ptr - ptr) * block_size;
        }

        reference operator*() const {
            std::cerr << "r* ";
            return *(*ptr + idx);
        }
        pointer operator->() const {
            std::cerr << "r-> ";
            return *ptr + idx;
        }
    };

    typedef Iterator<false> iterator;
    typedef Iterator<true> const_iterator;
    typedef ReverseIterator<false> reverse_iterator;
    typedef ReverseIterator<true> const_reverse_iterator;

    iterator begin() {
        return iterator(&data[outer_Begin], inner_Begin);
    }
    iterator end() {
        return ++iterator(&data[outer_End], inner_End);
    }
    const_iterator begin() const {
        return const_iterator(const_cast<T**>(&data[outer_Begin]), inner_Begin);
    }
    const_iterator end() const {
        return ++const_iterator(const_cast<T**>(&data[outer_End]), inner_End);
    }
    const_iterator cbegin() const {
        return const_iterator(const_cast<T**>(&data[outer_Begin]), inner_Begin);
    }
    const_iterator cend() const {
        return ++const_iterator(const_cast<T**>(&data[outer_End]), inner_End);
    }
    reverse_iterator rbegin() {
        return reverse_iterator(&data[outer_End], inner_End);
    }
    reverse_iterator rend() {
        return --reverse_iterator(&data[outer_Begin], inner_Begin);
    }
    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(const_cast<T**>(&data[outer_End]), inner_End);
    }
    const_reverse_iterator rend() const {
        return --const_reverse_iterator(const_cast<T**>(&data[outer_Begin]), inner_Begin);
    }
    const_reverse_iterator crbegin() const {
        return const_reverse_iterator(const_cast<T**>(&data[outer_End]), inner_End);
    }
    const_reverse_iterator crend() const {
        return --const_reverse_iterator(const_cast<T**>(&data[outer_Begin]), inner_Begin);
    }

    Deque() {
        data = {};
        capacity = 0;
        outer_Begin = -1;
        outer_End = -1;
        inner_Begin = -1;
        inner_End = -1;
    }
    Deque(const Deque<T>& deq) : Deque() {
        for (Deque<T>::const_iterator it = deq.begin(); it != deq.end(); ++it)
            push_back(*it);
    }
    Deque(int size) : Deque() {
        for (int i = 0; i < size; ++i)
            push_back(*reinterpret_cast<T*>(new uint8_t(sizeof(T))));
    }
    Deque(int size, const T& value) : Deque() {
        for (int i = 0; i < size; ++i)
            push_back(value);
    }
    ~Deque() {
        if (data.empty())
            return;
        for (Deque<T>::const_iterator it = cbegin(); it != cend(); ++it)
            it->~T();
        for (size_t i = outer_Begin; i <= outer_End; ++i)
            delete[] reinterpret_cast<uint8_t*>(data[i]);
    }
    Deque& operator=(const Deque<T>& deq) {
        data = {};
        capacity = 0;
        outer_Begin = -1;
        outer_End = -1;
        inner_Begin = -1;
        inner_End = -1;
        for (Deque<T>::const_iterator it = deq.begin(); it != deq.end(); ++it)
            push_back(*it);
        return *this;
    }

    size_t size() const {
        return end() - begin();
    }

    T& operator[](size_t index) {
        std::cerr << "[" << index << "] ";
        return *(begin() + index);
    }
    const T& operator[](size_t index) const {
        std::cerr << "[" << index << "] ";
        return *(begin() + index);
    }
    T& at(int index) {
        if (begin() + index >= end() || index < 0)
            throw std::out_of_range("Deque out of range");
        return this->operator[](index);
    }
    const T& at(int index) const {
        if (begin() + index >= end() || index < 0)
            throw std::out_of_range("Deque out of range");
        return this->operator[](index);
    }

    void push_back(const T& value) {
        if(capacity == 0) {
            add_block_back();
            outer_End = 1;
            outer_Begin = 1;
            inner_Begin = 0;
            inner_End = 0;
        } else if (inner_End == block_size - 1) {
            add_block_back();
            ++outer_End;
            inner_End = 0;
        } else {
            ++inner_End;
        }
        new (data[outer_End] + inner_End) T(value);
    }
    void push_front(const T& value) {
        if(capacity == 0) {
            outer_Begin = 1;
            add_block_front();
            outer_Begin = 1;
            outer_End = 1;
            inner_Begin = 0;
            inner_End = 0;
        } else if (inner_Begin == 0) {
            add_block_front();
            --outer_Begin;
            inner_Begin = block_size - 1;
        } else {
            --inner_Begin;
        }
        new (data[outer_Begin] + inner_Begin) T(value);
    }
    void pop_back() {
        if (begin() == --end())
            (--end())->~T();
        if (inner_End == 0) {
            delete[] reinterpret_cast<uint8_t*>(data[outer_End]);
            data[outer_End] = nullptr;
            --outer_End;
            inner_End = block_size - 1;
        }
        else
            --inner_End;
    }
    void pop_front() {
        if (begin() == --end())
            begin()->~T();
        if (inner_Begin == block_size - 1) {
            delete[] reinterpret_cast<uint8_t*>(data[outer_Begin]);
            data[outer_Begin] = nullptr;
            ++outer_Begin;
            inner_Begin = 0;
        }
        else
            ++inner_Begin;
    }

    void insert(iterator it, const T& value) {
        std::cerr << "insert" << " ";
        push_back(value);
        iterator iter = end() - 1;
        while (iter > it) {
            T tempr = *(iter - 1);
            *(iter - 1) = *iter;
            *iter = tempr;
            --iter;
        }
    }
    void erase(iterator it) {
        std::cerr << "erase" << " ";
        iterator iter = it;
        while (iter < end() - 1) {
            T tempr = *(iter + 1);
            *(iter + 1) = *iter;
            *iter = tempr;
            ++iter;
        }
        pop_back();
    }
};
