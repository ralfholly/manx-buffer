#include "ring_buffer.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <numeric>

#include "gmock/gmock.h"
using namespace testing;

namespace testing {
namespace ring_buffer_test {

TEST(ring_buffer, simple_instatiation) {
    RingBuffer<double, 10> rg;
    EXPECT_EQ(10U, rg.capacity());
    EXPECT_EQ(0U, rg.size());
    EXPECT_TRUE(rg.empty());
}

TEST(ring_buffer, simple_add_remove) {
    RingBuffer<double, 10> rg;
    rg.add(1.1);
    EXPECT_FALSE(rg.empty());
    EXPECT_EQ(1U, rg.size());
    EXPECT_EQ(1.1, rg.remove());
    EXPECT_TRUE(rg.empty());
    EXPECT_EQ(0U, rg.size());
}

TEST(ring_buffer, multi_add) {
    RingBuffer<int, 10> rg;
    rg.add(1);
    rg.add(2);
    rg.add(3);
    rg.add(4);
    rg.add(5);
    EXPECT_EQ(5U, rg.size());
    EXPECT_FALSE(rg.empty());
    EXPECT_EQ(1, rg.remove());
    EXPECT_EQ(2, rg.remove());
    EXPECT_EQ(3, rg.remove());
    EXPECT_EQ(4, rg.remove());
    EXPECT_EQ(5, rg.remove());
    EXPECT_EQ(0U, rg.size());
    EXPECT_TRUE(rg.empty());
}

TEST(ring_buffer, overflow1) {
    RingBuffer<int, 5> rg;
    rg.add(1);
    rg.add(2);
    rg.add(3);
    rg.add(4);
    rg.add(5);
    EXPECT_EQ(5U, rg.size());
    // The last add nudged the tail such that the value 1 (the oldest value)
    // doesn't exist anymore in the ring buffer.
    EXPECT_EQ(1, rg.remove());
    EXPECT_EQ(4U, rg.size());
    EXPECT_EQ(2, rg.remove());
    EXPECT_EQ(3U, rg.size());
    EXPECT_EQ(3, rg.remove());
    EXPECT_EQ(2U, rg.size());
    EXPECT_EQ(4, rg.remove());
    EXPECT_EQ(1U, rg.size());
    EXPECT_EQ(5, rg.remove());
    EXPECT_EQ(0U, rg.size());
    EXPECT_TRUE(rg.empty());
}

TEST(ring_buffer, overflow2) {
    RingBuffer<int, 5> rg;
    rg.add(1);
    rg.add(2);
    rg.add(3);
    rg.remove();
    rg.remove();
    rg.remove();
    rg.add(4);
    EXPECT_EQ(1U, rg.size());
    rg.add(5);
    EXPECT_EQ(2U, rg.size());
    rg.add(6);
    EXPECT_EQ(3U, rg.size());
    rg.add(7);
    EXPECT_EQ(4U, rg.size());
    rg.add(8);
    EXPECT_EQ(5U, rg.size());
    rg.add(9);
    EXPECT_EQ(5U, rg.size());
    rg.add(10);
    EXPECT_EQ(5U, rg.size());

    EXPECT_EQ(6, rg.remove());
    EXPECT_EQ(4U, rg.size());
    EXPECT_EQ(7, rg.remove());
    EXPECT_EQ(3U, rg.size());
    EXPECT_EQ(8, rg.remove());
    EXPECT_EQ(2U, rg.size());
    EXPECT_EQ(9, rg.remove());
    EXPECT_EQ(1U, rg.size());
    EXPECT_EQ(10, rg.remove());
    EXPECT_EQ(0U, rg.size());
}

TEST(ring_buffer, fill_refill) {
    RingBuffer<int, 5> rg;
    rg.add(1);
    rg.add(2);
    rg.add(3);
    EXPECT_EQ(1, rg.remove());
    EXPECT_EQ(2, rg.remove());
    EXPECT_EQ(3, rg.remove());
    EXPECT_TRUE(rg.empty());
    rg.add(1);
    rg.add(2);
    rg.add(3);
    EXPECT_EQ(1, rg.remove());
    EXPECT_EQ(2, rg.remove());
    EXPECT_EQ(3, rg.remove());
    EXPECT_TRUE(rg.empty());
}

TEST(ring_buffer, clear) {
    RingBuffer<int, 5> rg;
    rg.add(1);
    rg.add(2);
    rg.add(3);
    EXPECT_FALSE(rg.empty());
    rg.clear();
    EXPECT_TRUE(rg.empty());
}

TEST(ring_buffer, delete_one_by_one) {
    RingBuffer<int, 10> rb;

    rb.add(123);
    rb.add(42);
    rb.add(23);
    assert(rb.size() == 3);

    while (!rb.empty()) {
        std::cout << rb.remove() << std::endl;
    }
    assert(rb.size() == 0);
}

TEST(ring_buffer, basic_iterator_usage) {
    RingBuffer<int, 3> rb;

    rb.add(1);
    rb.add(2);
    rb.add(3);

    // Preincrement.
    auto it = rb.begin();
    ASSERT_EQ(1, *it);
    ASSERT_EQ(2, *++it);
    ASSERT_EQ(3, *++it);

    // Postincrement.
    it = rb.begin();
    ASSERT_EQ(1, *it++);
    ASSERT_EQ(2, *it++);
    ASSERT_EQ(3, *it++);

    // Wrap-around.
    rb.add(4);
    it = rb.begin();
    ASSERT_EQ(2, *it++);
    ASSERT_EQ(3, *it++);
    ASSERT_EQ(4, *it++);
}

TEST(ring_buffer, const_iterator_usage) {
    RingBuffer<int, 3> rb;

    rb.add(1);
    rb.add(2);
    rb.add(3);

    auto it = rb.cbegin();
    static_assert(std::is_same_v<RingBuffer<int, 3>::ConstIteratorType, decltype(it)>);

    // Preincrement.
    ASSERT_EQ(1, *it);
    ASSERT_EQ(2, *++it);
    ASSERT_EQ(3, *++it);

    // Postincrement.
    it = rb.cbegin();
    ASSERT_EQ(1, *it++);
    ASSERT_EQ(2, *it++);
    ASSERT_EQ(3, *it++);

    // Wrap-around.
    rb.add(4);
    it = rb.cbegin();
    ASSERT_EQ(2, *it++);
    ASSERT_EQ(3, *it++);
    ASSERT_EQ(4, *it++);
}

TEST(ring_buffer, iterator_from_begin_to_end) {
    RingBuffer<int, 3> rb;

    // Empty ring buffer.
    ASSERT_EQ(rb.begin(), rb.end());

    // Fill with overwrite.
    rb.add(1);
    rb.add(2);
    rb.add(3);
    rb.add(4);

    // Plain old for loop.
    int count = 0;
    for (auto it = rb.begin(); it != rb.end(); ++it) {
        switch (++count) {
        case 1:
            ASSERT_EQ(2, *it);
            break;
        case 2:
            ASSERT_EQ(3, *it);
            break;
        case 3:
            ASSERT_EQ(4, *it);
            break;
        default:
            ASSERT_TRUE(false) << "Unexpected ring buffer element";
        }
    }

    // Range-based for loop.
    count = 0;
    for (int i : rb) {
        switch (++count) {
        case 1:
            ASSERT_EQ(2, i);
            break;
        case 2:
            ASSERT_EQ(3, i);
            break;
        case 3:
            ASSERT_EQ(4, i);
            break;
        default:
            ASSERT_TRUE(false) << "Unexpected ring buffer element";
        }
    }

    // Consume all but one element.
    rb.remove();
    rb.remove();
    ASSERT_EQ(1U, rb.size());
    count = 0;
    for (int i : rb) {
        ++count;
        ASSERT_EQ(1, count);
        ASSERT_EQ(4, i);
    }

    // Ring buffer empty.
    ASSERT_EQ(4, rb.remove());
    for ([[maybe_unused]] int i : rb) {
        ASSERT_FALSE(true);
    }
    ASSERT_EQ(rb.begin(), rb.end());
}

TEST(ring_buffer, iterator_stl_usage) {
    RingBuffer<int, 3> rb;

    // Fill with overwrite.
    rb.add(1);
    rb.add(2);
    rb.add(3);
    rb.add(4);

    ASSERT_EQ(2 + 3 + 4, std::accumulate(rb.begin(), rb.end(), 0));
    ASSERT_EQ(3, *std::find(rb.begin(), rb.end(), 3));
    ASSERT_TRUE(std::all_of(rb.begin(), rb.end(), [](int i) { return i < 10; }));
    ASSERT_FALSE(std::all_of(rb.begin(), rb.end(), [](int i) { return i > 10; }));
    ASSERT_TRUE(std::any_of(rb.begin(), rb.end(), [](int i) { return (i % 2) != 0; }));
    std::fill(rb.begin(), rb.end(), 42);
    ASSERT_TRUE(std::all_of(rb.begin(), rb.end(), [](int i) { return i == 42; }));
    std::vector<int> vec;
    std::copy(rb.begin(), rb.end(), std::back_inserter(vec));
    ASSERT_EQ(2, vec[0]);
    ASSERT_EQ(3, vec[1]);
    ASSERT_EQ(4, vec[2]);
}

TEST(ring_buffer, const_iterator_stl_usage) {
    RingBuffer<int, 3> rb;

    // Fill with overwrite.
    rb.add(1);
    rb.add(2);
    rb.add(3);
    rb.add(4);

    ASSERT_EQ(2 + 3 + 4, std::accumulate(rb.cbegin(), rb.cend(), 0));
    ASSERT_EQ(3, *std::find(rb.cbegin(), rb.cend(), 3));
    ASSERT_TRUE(std::all_of(rb.cbegin(), rb.cend(), [](int i) { return i < 10; }));
    ASSERT_FALSE(std::all_of(rb.cbegin(), rb.cend(), [](int i) { return i > 10; }));
    ASSERT_TRUE(std::any_of(rb.cbegin(), rb.cend(), [](int i) { return (i % 2) != 0; }));
    ASSERT_TRUE(std::none_of(rb.cbegin(), rb.cend(), [](int i) { return i == 42; }));
    std::vector<int> vec;
    std::copy(rb.cbegin(), rb.cend(), std::back_inserter(vec));
    ASSERT_EQ(2, vec[0]);
    ASSERT_EQ(3, vec[1]);
    ASSERT_EQ(4, vec[2]);
}

} // namespace ring_buffer
} // namespace testing
