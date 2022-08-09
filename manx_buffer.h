#ifndef MANX_BUFFER_H__
#define MANX_BUFFER_H__

#include <cstddef>
#include <cassert>
#include <iterator>
#include <array>

template<typename EntryType, int N>
class ManxBuffer {
public:
    template <typename ManxBufferType, typename T> class Iterator {
    public:
        using difference_type = ptrdiff_t;
        using value_type = T;
        using reference = T&;
        using pointer = T*;
        using iterator_category = std::forward_iterator_tag;

        Iterator(ManxBufferType* manxBuffer, size_t pos)
            : manxBuffer_(manxBuffer)
            , pos_(pos) {}
        T& operator*() const { return (manxBuffer_->buffer_)[pos_]; }
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

    using IteratorType = Iterator<ManxBuffer, EntryType>;
    using ConstIteratorType = Iterator<const ManxBuffer, const EntryType>;

    ManxBuffer() = default;

    explicit ManxBuffer(const EntryType& initialValue) {
        std::fill(buffer_.begin(), buffer_.end(), initialValue);
    }

    [[nodiscard]] IteratorType begin() { return IteratorType(this, ManxBuffer::successor(head_)); }
    [[nodiscard]] IteratorType end() { return IteratorType(this, head_); }

    [[nodiscard]] ConstIteratorType begin() const { return ConstIteratorType(this, ManxBuffer::successor(head_)); }
    [[nodiscard]] ConstIteratorType end() const { return ConstIteratorType(this, head_); }

    [[nodiscard]] ConstIteratorType cbegin() const { return ConstIteratorType(this, ManxBuffer::successor(head_)); }
    [[nodiscard]] ConstIteratorType cend() const { return ConstIteratorType(this, head_); }

    size_t capacity() const { return N; }

    void add(const EntryType& item) {
        buffer_[head_] = item;
        advance(head_);
    }

private:
    static constexpr size_t BUFSIZE = N + 1U;
    static size_t successor(size_t value) { return (value + 1) % BUFSIZE; }
    static void advance(size_t& value) { value = successor(value); }

    std::array<EntryType, BUFSIZE> buffer_{};
    size_t head_{0U};
};

#endif
