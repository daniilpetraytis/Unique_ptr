#pragma once

#include <cstddef>  // std::nullptr_t
#include <type_traits>
#include <utility>
#include <compressed_pair.h>
template <typename T>
struct Slug {
    Slug() {
    }
    void operator()(T* p) const {
        if (p) {
            delete p;
        }
    }
    template <typename D>
    Slug(D deleter) {
    }
};

// Primary template
template <typename T, typename Deleter = Slug<T>>
class UniquePtr {
private:
    CompressedPair<T*, Deleter> data_;
    template <typename Tp, typename Deleter1>
    friend class UniquePtr;
    void operator=(const UniquePtr&);

public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors
    explicit UniquePtr(T* ptr = nullptr) : data_(ptr, Deleter()) {
    }
    template <typename Deleter1>
    UniquePtr(T* ptr, Deleter1&& deleter) : data_(ptr, std::forward<Deleter1>(deleter)) {
    }

    template <typename Tp, typename Deleter1>
    UniquePtr(UniquePtr<Tp, Deleter1>&& other) noexcept
        : data_(other.data_.GetFirst(), std::forward<Deleter1>(other.data_.GetSecond())) {
        other.data_.GetFirst() = nullptr;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s
    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (&other == this) {
            return *this;
        }
        data_.GetSecond()(data_.GetFirst());
        data_.GetFirst() = other.data_.GetFirst();
        data_.GetSecond() = std::forward<Deleter>(other.GetDeleter());
        other.data_.GetFirst() = nullptr;
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) {
        data_.GetSecond()(data_.GetFirst());
        data_.GetFirst() = nullptr;
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() {
        T* x = data_.GetFirst();
        data_.GetFirst() = nullptr;
        if (x) {
            data_.GetSecond()(x);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    T* Release() {
        T* ptr = data_.GetFirst();
        data_.GetFirst() = nullptr;
        return ptr;
    }
    void Reset(T* ptr = nullptr) {
        if (data_.GetFirst() != ptr) {
            T* x = data_.GetFirst();
            data_.GetFirst() = ptr;
            if (x) {
                data_.GetSecond()(x);
            }
        }
    }
    void Swap(UniquePtr& other) {
        std::swap(other, *this);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {
        return data_.GetFirst();
    }
    Deleter& GetDeleter() {
        return data_.GetSecond();
    }
    const Deleter& GetDeleter() const {
        return data_.GetSecond();
    }
    explicit operator bool() const {
        return data_.GetFirst() != nullptr;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Single-object dereference operators

    std::add_lvalue_reference_t<T> operator*() const {
        return *data_.GetFirst();
    }
    const T* operator->() const {
        return data_.GetFirst();
    }
};

// Specialization for arrays

template <typename T>
struct Slug<T[]> {
    Slug() {
    }
    void operator()(T* p) const {
        if (p) {
            delete[] p;
        }
    }
    template <typename D>
    Slug(D deleter) {
    }
};

template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
private:
    CompressedPair<T*, Deleter> data_;
    template <typename Tp, typename Deleter1>
    friend class UniquePtr;
    void operator=(const UniquePtr&);

public:
    explicit UniquePtr(T* ptr = nullptr) : data_(ptr, Deleter()) {
    }
    template <typename Deleter1>
    UniquePtr(T* ptr, Deleter1&& deleter) : data_(ptr, std::forward<Deleter1>(deleter)) {
    }

    template <typename Tp, typename Deleter1>
    UniquePtr(UniquePtr<Tp, Deleter1>&& other) noexcept
        : data_(other.data_.GetFirst(), std::forward<Deleter1>(other.data_.GetSecond())) {
        other.data_.GetFirst() = nullptr;
    }

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (&other == this) {
            return *this;
        }
        data_.GetSecond()(data_.GetFirst());
        data_.GetFirst() = other.data_.GetFirst();
        data_.GetSecond() = std::forward<Deleter>(other.GetDeleter());
        other.data_.GetFirst() = nullptr;
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t) {
        data_.GetSecond()(data_.GetFirst());
        data_.GetFirst() = nullptr;
        return *this;
    }

    ~UniquePtr() {
        T* x = data_.GetFirst();
        data_.GetFirst() = nullptr;
        if (x) {
            data_.GetSecond()(x);
        }
    }

    T* Release() {
        T* ptr = data_.GetFirst();
        data_.GetFirst() = nullptr;
        return ptr;
    }
    void Reset(T* ptr = nullptr) {
        if (data_.GetFirst() != ptr) {
            T* x = data_.GetFirst();
            data_.GetFirst() = ptr;
            if (x) {
                data_.GetSecond()(x);
            }
        }
    }
    void Swap(UniquePtr& other) {
        std::swap(other, *this);
    }

    T* Get() const {
        return data_.GetFirst();
    }
    Deleter& GetDeleter() {
        return data_.GetSecond();
    }
    const Deleter& GetDeleter() const {
        return data_.GetSecond();
    }
    explicit operator bool() const {
        return data_.GetFirst() != nullptr;
    }

    std::add_lvalue_reference_t<T> operator*() const {
        return *data_.GetFirst();
    }
    T* operator->() const {
        return data_.GetFirst();
    }

    T& operator[](size_t i) {
        return data_.GetFirst()[i];
    };

    T& operator[](size_t i) const {
        return data_.GetFirst()[i];
    };
};