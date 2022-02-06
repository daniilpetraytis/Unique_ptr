#pragma once

#include <type_traits>
#include <cstddef>
#include <utility>

template <typename T, size_t I, bool = std::is_empty_v<T> && !std::is_final_v<T>>
struct CompressedPairElement {
    // Constructors.
    // Getters: return value.
    T value_;
    template <typename Tp>
    CompressedPairElement(Tp&& x) : value_(std::forward<Tp>(x)) {
    }
    CompressedPairElement() {
        value_ = T();
    }
    const T& Get() const {
        return value_;
    }
    T& Get() {
        return value_;
    }
};

template <typename T, size_t I>
struct CompressedPairElement<T, I, true> : public T {
    // Constructors.
    // Getters: return *this.
    CompressedPairElement() {
    }
    template <typename Tp>
    CompressedPairElement(Tp&& x) : T(std::forward<Tp>(x)) {
    }

    const T& Get() const {
        return *this;
    }
    T& Get() {
        return *this;
    }
};

// Me think, why waste time write lot code, when few code do trick.
template <typename F, typename S>
class CompressedPair : private CompressedPairElement<F, 0>, private CompressedPairElement<S, 1> {
    using First = CompressedPairElement<F, 0>;
    using Second = CompressedPairElement<S, 1>;

public:
    CompressedPair(){};
    template <typename Fp, typename Sp>
    CompressedPair(Fp&& f, Sp&& s) : First(std::forward<Fp>(f)), Second(std::forward<Sp>(s)){};

    F& GetFirst() {
        return First::Get();
    }
    S& GetSecond() {
        return Second::Get();
    };

    const S& GetSecond() const {
        return Second::Get();
    };

    const F& GetFirst() const {
        return First::Get();
    }
};