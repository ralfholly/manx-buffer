#ifndef RING_BUFFER_H__
#define RING_BUFFER_H__

#include <cstddef>
#include <cassert>
#include <iterator>

template<typename EntryType, int N>
class RingBuffer {
public:
    template <typename RingBufferType, typename T> class Iterator {
    public:
        using difference_type = ptrdiff_t;
        using value_type = T;
        using reference = T&;
        using pointer = T*;
        using iterator_category = std::forward_iterator_tag;

        Iterator(RingBufferType* ringBuffer, size_t pos)
            : m_ringBuffer(ringBuffer)
            , m_pos(pos) {}
        T& operator*() const { return (m_ringBuffer->buffer_)[m_pos]; }
        Iterator& operator++() {
            RingBuffer::advance(m_pos);
            return *this;
        }
        Iterator operator++(int) {
            auto old = *this;
            RingBuffer::advance(m_pos);
            return old;
        }
        bool operator==(const Iterator& rhs) const { return m_pos == rhs.m_pos; }
        bool operator!=(const Iterator& rhs) const { return !(*this == rhs); }

    private:
        RingBufferType* m_ringBuffer;
        size_t m_pos;
    };

    using IteratorType = Iterator<RingBuffer, EntryType>;
    using ConstIteratorType = Iterator<const RingBuffer, const EntryType>;

    [[nodiscard]] IteratorType begin() { return IteratorType(this, tail_); }
    [[nodiscard]] IteratorType end() { return IteratorType(this, head_); }

    [[nodiscard]] ConstIteratorType begin() const { return ConstIteratorType(this, tail_); }
    [[nodiscard]] ConstIteratorType end() const { return ConstIteratorType(this, head_); }

    [[nodiscard]] ConstIteratorType cbegin() const { return ConstIteratorType(this, tail_); }
    [[nodiscard]] ConstIteratorType cend() const { return ConstIteratorType(this, head_); }

    RingBuffer() { clear(); }
    size_t capacity() const { return N; }
    bool empty() const {
        return head_ == tail_;
    }
    void add(const EntryType& item) {
        buffer_[head_] = item;
        advance(head_);
        if (head_ == tail_) {
            advance(tail_); // Drop oldest entry, keep rest.
        }
    }
    size_t size() const {
        return (head_ - tail_ + BUFSIZE) % BUFSIZE; 
    }
    const EntryType& remove() {
        assert(!empty());
        size_t old_tail = tail_;
        advance(tail_);
        return buffer_[old_tail];
    }
    void clear() { 
        tail_ = head_ = 0U;
    }

private:
    static const size_t BUFSIZE = N + 1U;
    static void advance(size_t& value) { value = (value + 1) % BUFSIZE; }

    EntryType buffer_[BUFSIZE];
    size_t head_{0U};
    size_t tail_{0U};
};

#endif
