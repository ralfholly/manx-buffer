#ifndef MANX_BUFFER_H__
#define MANX_BUFFER_H__

#include <cstddef>
#include <iterator>
#include <array>

template<typename T, int N>
class ManxBuffer {
public:
    template <typename ManxBufferType, typename EntryType> class Iterator {
    public:
        using difference_type = ptrdiff_t;
        using value_type = EntryType;
        using reference = EntryType&;
        using pointer = EntryType*;
        using iterator_category = std::forward_iterator_tag;

        Iterator(ManxBufferType* manxBuffer, size_t pos)
            : manxBuffer_(manxBuffer)
            , pos_(pos) {}
        EntryType& operator*() const { return (manxBuffer_->buffer_)[pos_]; }
        Iterator& operator++() {
            ManxBuffer::advance(pos_);
            return *this;
        }
        Iterator operator++(int) {
            auto old = *this;
            ManxBuffer::advance(pos_);
            return old;
        }
        bool operator==(const Iterator& rhs) const { return pos_ == rhs.pos_; }
        bool operator!=(const Iterator& rhs) const { return !(*this == rhs); }

    private:
        ManxBufferType* manxBuffer_;
        size_t pos_;
    };

    using IteratorType = Iterator<ManxBuffer, T>;
    using ConstIteratorType = Iterator<const ManxBuffer, const T>;

    ManxBuffer() = default;

    explicit ManxBuffer(const T& initialValue) {
        std::fill(buffer_.begin(), buffer_.end(), initialValue);
    }

    [[nodiscard]] IteratorType begin() { return IteratorType(this, ManxBuffer::successor(head_)); }
    [[nodiscard]] IteratorType end() { return IteratorType(this, head_); }

    [[nodiscard]] ConstIteratorType begin() const { return ConstIteratorType(this, ManxBuffer::successor(head_)); }
    [[nodiscard]] ConstIteratorType end() const { return ConstIteratorType(this, head_); }

    [[nodiscard]] ConstIteratorType cbegin() const { return ConstIteratorType(this, ManxBuffer::successor(head_)); }
    [[nodiscard]] ConstIteratorType cend() const { return ConstIteratorType(this, head_); }

    [[nodiscard]] constexpr size_t capacity() const { return N; }

    void add(const T& item) {
        buffer_[head_] = item;
        advance(head_);
    }

private:
    static constexpr size_t BUFSIZE = N + 1U;

    static size_t successor(size_t value) { return (value + 1) % BUFSIZE; }
    static void advance(size_t& value) { value = successor(value); }

    std::array<T, BUFSIZE> buffer_{};
    size_t head_{0U};
};

#endif
