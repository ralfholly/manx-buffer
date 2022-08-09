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
    RingBuffer<int, 3> rg;
    EXPECT_EQ(3U, rg.capacity());
}

TEST(ring_buffer, simple_add) {
    RingBuffer<int, 3> rg;
    rg.add(42);
    auto it = rg.cbegin();
    ASSERT_EQ(0, *it++);
    ASSERT_EQ(0, *it++);
    ASSERT_EQ(42, *it++);
    ASSERT_EQ(it, rg.cend());
}

TEST(ring_buffer, default_value) {
    static constexpr int defaultValue = -999;
    RingBuffer<int, 3> rg{defaultValue};
    rg.add(42);
    auto it = rg.cbegin();
    ASSERT_EQ(defaultValue, *it++);
    ASSERT_EQ(defaultValue, *it++);
    ASSERT_EQ(42, *it++);
    ASSERT_EQ(it, rg.cend());
}

TEST(ring_buffer, multi_add) {
    RingBuffer<int, 5> rg;
    rg.add(1);
    rg.add(2);
    rg.add(3);
    rg.add(4);
    auto it = rg.cbegin();
    ASSERT_EQ(0, *it++);
    ASSERT_EQ(1, *it++);
    ASSERT_EQ(2, *it++);
    ASSERT_EQ(3, *it++);
    ASSERT_EQ(4, *it++);
    ASSERT_EQ(it, rg.cend());
}

TEST(ring_buffer, multi_add_overflow) {
    RingBuffer<int, 5> rg;
    rg.add(1);
    rg.add(2);
    rg.add(3);
    rg.add(4);
    rg.add(5);
    rg.add(6);
    auto it = rg.cbegin();
    ASSERT_EQ(2, *it++);
    ASSERT_EQ(3, *it++);
    ASSERT_EQ(4, *it++);
    ASSERT_EQ(5, *it++);
    ASSERT_EQ(6, *it++);
    ASSERT_EQ(it, rg.cend());
}

TEST(ring_buffer, iterator_constness) {
    RingBuffer<int, 5> rg;
    static_assert(std::is_const_v<std::remove_reference_t<decltype(*(rg.cbegin()))>>);
    static_assert(std::is_const_v<std::remove_reference_t<decltype(*(rg.cend()))>>);
    static_assert(!std::is_const_v<std::remove_reference_t<decltype(*(rg.begin()))>>);
    static_assert(!std::is_const_v<std::remove_reference_t<decltype(*(rg.end()))>>);

    const RingBuffer<int, 5> crg;
    static_assert(std::is_const_v<std::remove_reference_t<decltype(*(crg.cbegin()))>>);
    static_assert(std::is_const_v<std::remove_reference_t<decltype(*(crg.cend()))>>);
    static_assert(std::is_const_v<std::remove_reference_t<decltype(*(crg.begin()))>>);
    static_assert(std::is_const_v<std::remove_reference_t<decltype(*(crg.end()))>>);
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
    std::vector<int> vec;
    std::copy(rb.begin(), rb.end(), std::back_inserter(vec));
    ASSERT_EQ(2, vec[0]);
    ASSERT_EQ(3, vec[1]);
    ASSERT_EQ(4, vec[2]);
    std::fill(rb.begin(), rb.end(), 42);
    ASSERT_TRUE(std::all_of(rb.begin(), rb.end(), [](int i) { return i == 42; }));
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

TEST(ring_buffer, string_elements_empty_default) {
    RingBuffer<std::string, 3> rg;
    static const std::string someString = "One";
    rg.add(someString);
    auto it = rg.cbegin();
    ASSERT_EQ("", *it++);
    ASSERT_EQ("", *it++);
    ASSERT_EQ(someString, *it++);
    ASSERT_EQ(it, rg.cend());
}

TEST(ring_buffer, string_elements_empty_special_default_value) {
    static const std::string defaultValue = "empty";
    RingBuffer<std::string, 3> rg{defaultValue};
    static const std::string someString = "One";
    rg.add(someString);
    auto it = rg.cbegin();
    ASSERT_EQ(defaultValue, *it++);
    ASSERT_EQ(defaultValue, *it++);
    ASSERT_EQ(someString, *it++);
    ASSERT_EQ(it, rg.cend());
}

TEST(ring_buffer, copy_construction) {
    RingBuffer<int, 3> rb1;
    rb1.add(1);
    rb1.add(2);
    rb1.add(3);
    rb1.add(4);
    RingBuffer<int, 3> rb2(rb1);
    auto it1 = rb1.begin();
    auto it2 = rb2.begin();
    while (it1 != rb1.end()) {
        ASSERT_EQ(*it2++, *it1++);
    }
    ASSERT_EQ(rb1.end(), it1);
    ASSERT_EQ(rb2.end(), it2);
}

} // namespace ring_buffer
} // namespace testing
