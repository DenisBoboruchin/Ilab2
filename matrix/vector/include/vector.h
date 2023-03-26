#ifndef VECTOR_H
#define VECTOR_H

#include <cassert>
#include <cstdint>
#include <cstring>
#include <exception>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace my_containers {

template <typename T, typename Alloc = std::allocator<T>>
class vector {
    static_assert(std::is_nothrow_move_constructible_v<T>);
    static_assert(std::is_nothrow_move_assignable_v<T>);
    static_assert(std::is_nothrow_destructible_v<T>);

public:
    vector() = default;
    vector(size_t n, const T &value) : capacity_(new_cap(n)), arr(All(capacity_)), size_(n)
    {
        for (std::size_t i = 0; i < size_; i++) {
            new (arr + i) T(value);
        }
    }
    explicit vector(std::size_t n) : capacity_(new_cap(n)), arr(All(capacity_)), size_(n)
    {
        for (std::size_t i = 0; i < size_; i++) {
            new (arr + i) T();
        }
    }
    vector(const vector &other) : capacity_(new_cap(other.size_)), arr(All(capacity_)), size_(other.size_)
    {
        for (std::size_t i = 0; i < size_; i++) {
            try {
                new (arr + i) T(other.arr[i]);
            } catch (...) {
                for (std::size_t j = 0; j < i; j++) {
                    arr[j].~T();
                }
                Alloc().deallocate(arr, capacity_);
                throw;
            }
        }
    }
    vector(vector &&other) noexcept
        : capacity_(std::exchange(other.capacity_, 0))
        , arr(std::exchange(other.arr, nullptr))
        , size_(std::exchange(other.size_, 0))
    {
    }
    ~vector()
    {
        del(0, size_, arr);
        if (capacity_ != 0) {
            Alloc().deallocate(arr, capacity_);
        }
        size_ = 0;
        capacity_ = 0;
    }
    vector &operator=(vector &&other) noexcept
    {
        if (this != &other) {
            if (capacity_ != 0) {
                for (std::size_t i = 0; i < size_; i++) {
                    arr[i].~T();
                }
                Alloc().deallocate(arr, capacity_);
            }
            std::swap(capacity_, other.capacity_);
            arr = std::exchange(other.arr, nullptr);
            size_ = std::exchange(other.size_, 0);
        }
        return *this;
    }
    vector &operator=(const vector &other)
    {
        if (this != &other) {
            std::size_t tmp_capacity_ = new_cap(other.size_);
            T *buf = All(tmp_capacity_);
            for (std::size_t i = 0; i < other.size_; i++) {
                try {
                    new (buf + i) T(other.arr[i]);
                } catch (...) {
                    for (std::size_t j = 0; j < i; j++) {
                        buf[j].~T();
                    }
                    Alloc().deallocate(buf, tmp_capacity_);
                    throw;
                }
            }
            if (capacity_ != 0) {
                for (std::size_t i = 0; i < size_; i++) {
                    arr[i].~T();
                }
                Alloc().deallocate(arr, capacity_);
            }
            std::swap(arr, buf);
            capacity_ = tmp_capacity_;
            size_ = other.size_;
        }
        return *this;
    }
    [[nodiscard]] std::size_t size() const noexcept
    {
        return size_;
    }
    [[nodiscard]] bool empty() const noexcept
    {
        return size_ == 0;
    }
    [[nodiscard]] std::size_t capacity() const noexcept
    {
        return capacity_;
    }
    T &operator[](std::size_t n) & noexcept
    {
        return arr[n];
    }
    T &&operator[](std::size_t n) && noexcept
    {
        return std::move(arr[n]);
    }
    const T &operator[](std::size_t n) const & noexcept
    {
        return arr[n];
    }
    T &at(std::size_t n) &
    {
        if (n >= size_) {
            throw std::out_of_range("123");
        }
        return arr[n];
    }
    T &&at(std::size_t n) &&
    {
        if (n >= size_) {
            throw std::out_of_range("123");
        }
        return std::move(arr[n]);
    }
    const T &at(std::size_t n) const &
    {
        if (n >= size_) {
            throw std::out_of_range("123");
        }
        return arr[n];
    }
    void reserve(std::size_t n) &
    {
        if (capacity_ < n) {
            std::size_t new_capacity = new_cap(n);
            T *buf = All(new_capacity);
            std::size_t i = 0;
            try {
                for (; i != size_; i++) {
                    new (buf + i) T(std::move(arr[i]));
                }
                std::swap(arr, buf);
                del(0, size_, buf);
                Alloc().deallocate(buf, capacity_);
                capacity_ = new_capacity;
            } catch (...) {
                del(0, i, buf);
                Alloc().deallocate(buf, new_capacity);
                throw;
            }
        }
    }
    void push_back(const T &value) &
    {
        try {
            resize(size_ + 1, value);
        } catch (...) {
            throw;
        }
    }
    void push_back(T &&value) &
    {
        if (size_ == capacity_) {
            if (capacity_ == 0) {
                capacity_ = 2;
                arr = All(capacity_);
            } else {
                std::size_t new_capacity_ = capacity_ * 2;
                T *buf = All(new_capacity_);
                std::size_t i = 0;
                try {
                    for (; i < size_; i++) {
                        new (buf + i) T(std::move(arr[i]));
                    }
                    Alloc().deallocate(arr, capacity_);
                    arr = buf;
                    capacity_ = new_capacity_;
                } catch (...) {
                    del(0, i, buf);
                    Alloc().deallocate(buf, new_capacity_);
                    throw;
                }
            }
        }
        try {
            new (arr + size_) T(std::move(value));
        } catch (...) {
            throw;
        }
        size_++;
    }
    void pop_back() & noexcept
    {
        size_--;
        arr[size_].~T();
    }
    void resize(std::size_t n) &
    {
        if (size_ < n) {
            std::size_t new_capacity = new_cap(n);
            T *buf = All(new_capacity);
            std::size_t i = size_;
            try {
                for (; i != n; i++) {
                    new (buf + i) T();
                }
                for (std::size_t j = 0; j < size_; j++) {
                    new (buf + j) T(std::move(arr[j]));
                }
                std::swap(arr, buf);
                del(0, size_, buf);
                Alloc().deallocate(buf, capacity_);
                capacity_ = new_capacity;
                size_ = n;
            } catch (...) {
                del(size_, i, buf);
                Alloc().deallocate(buf, new_capacity);
                throw;
            }
        } else if (size_ > n) {
            del(n, size_, arr);
            size_ = n;
        }
    }
    void resize(std::size_t n, const T &value) &
    {
        if (size_ < n) {
            std::size_t new_capacity = new_cap(n);
            T *buf = All(new_capacity);
            std::size_t i = size_;
            try {
                for (; i != n; i++) {
                    new (buf + i) T(value);
                }
                for (std::size_t j = 0; j < size_; j++) {
                    new (buf + j) T(std::move(arr[j]));
                }
                std::swap(arr, buf);
                del(0, size_, buf);
                Alloc().deallocate(buf, capacity_);
                capacity_ = new_capacity;
                size_ = n;
            } catch (...) {
                del(size_, i, buf);
                Alloc().deallocate(buf, new_capacity);
                throw;
            }
        } else if (size_ > n) {
            del(n, size_, arr);
            size_ = n;
        }
    }
    void clear() &
    {
        del(0, size_, arr);
        size_ = 0;
    }

private:
    std::size_t capacity_ = 0;
    T *arr = nullptr;
    std::size_t size_ = 0;
    std::size_t new_cap(std::size_t x)
    {
        if (x == 0) {
            return 0;
        }
        std::size_t y = 1;
        while (y < x) {
            y *= 2;
        }
        return y;
    }

    void del(std::size_t i, std::size_t j, T *buf) noexcept
    {
        for (std::size_t k = i; k < j; k++) {
            buf[k].~T();
        }
    }

    T *All(std::size_t n)
    {
        if (n == 0) {
            return nullptr;
        }
        return Alloc().allocate(n);
    }
};

}  // namespace my_containers

#endif
