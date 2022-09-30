#include "manx_buffer.h"

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
namespace manx_buffer_test {

TEST(manx_buffer_test, simple_instatiation) {
    ManxBuffer<int, 3> mb;
    EXPECT_EQ(3U, mb.capacity());
}

TEST(manx_buffer_test, simple_add) {
    ManxBuffer<int, 3> mb;
    mb.add(42);
    auto it = mb.cbegin();
    ASSERT_EQ(0, *it++);
    ASSERT_EQ(0, *it++);
    ASSERT_EQ(42, *it++);
    ASSERT_EQ(it, mb.cend());
}

TEST(manx_buffer_test, default_value) {
    static constexpr int defaultValue = -999;
    ManxBuffer<int, 3> mb{defaultValue};
    mb.add(42);
    auto it = mb.cbegin();
    ASSERT_EQ(defaultValue, *it++);
    ASSERT_EQ(defaultValue, *it++);
    ASSERT_EQ(42, *it++);
    ASSERT_EQ(it, mb.cend());
}

TEST(manx_buffer_test, multi_add) {
    ManxBuffer<int, 5> mb;
    mb.add(1);
    mb.add(2);
    mb.add(3);
    mb.add(4);
    auto it = mb.cbegin();
    ASSERT_EQ(0, *it++);
    ASSERT_EQ(1, *it++);
    ASSERT_EQ(2, *it++);
    ASSERT_EQ(3, *it++);
    ASSERT_EQ(4, *it++);
    ASSERT_EQ(it, mb.cend());
}

TEST(manx_buffer_test, multi_add_overflow) {
    ManxBuffer<int, 5> mb;
    mb.add(1);
    mb.add(2);
    mb.add(3);
    mb.add(4);
    mb.add(5);
    mb.add(6);
    auto it = mb.cbegin();
    ASSERT_EQ(2, *it++);
    ASSERT_EQ(3, *it++);
    ASSERT_EQ(4, *it++);
    ASSERT_EQ(5, *it++);
    ASSERT_EQ(6, *it++);
    ASSERT_EQ(it, mb.cend());
}

TEST(manx_buffer_test, iterator_constness) {
    ManxBuffer<int, 5> mb;
    static_assert(std::is_const_v<std::remove_reference_t<decltype(*(mb.cbegin()))>>);
    static_assert(std::is_const_v<std::remove_reference_t<decltype(*(mb.cend()))>>);
    static_assert(!std::is_const_v<std::remove_reference_t<decltype(*(mb.begin()))>>);
    static_assert(!std::is_const_v<std::remove_reference_t<decltype(*(mb.end()))>>);

    const ManxBuffer<int, 5> crg;
    static_assert(std::is_const_v<std::remove_reference_t<decltype(*(crg.cbegin()))>>);
    static_assert(std::is_const_v<std::remove_reference_t<decltype(*(crg.cend()))>>);
    static_assert(std::is_const_v<std::remove_reference_t<decltype(*(crg.begin()))>>);
    static_assert(std::is_const_v<std::remove_reference_t<decltype(*(crg.end()))>>);
}

TEST(manx_buffer_test, basic_iterator_usage) {
    ManxBuffer<int, 3> rb;

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

TEST(manx_buffer_test, const_iterator_usage) {
    ManxBuffer<int, 3> rb;

    rb.add(1);
    rb.add(2);
    rb.add(3);

    auto it = rb.cbegin();
    static_assert(std::is_same_v<ManxBuffer<int, 3>::ConstIteratorType, decltype(it)>);

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

TEST(manx_buffer_test, iterator_from_begin_to_end) {
    ManxBuffer<int, 3> rb;

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
            ASSERT_TRUE(false) << "Unexpected manx buffer element";
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
            ASSERT_TRUE(false) << "Unexpected manx buffer element";
        }
    }
}

TEST(manx_buffer_test, iterator_stl_usage) {
    ManxBuffer<int, 3> rb;

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

TEST(manx_buffer_test, const_iterator_stl_usage) {
    ManxBuffer<int, 3> rb;

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

TEST(manx_buffer_test, string_elements_empty_default) {
    ManxBuffer<std::string, 3> mb;
    static const std::string someString = "One";
    mb.add(someString);
    auto it = mb.cbegin();
    ASSERT_EQ("", *it++);
    ASSERT_EQ("", *it++);
    ASSERT_EQ(someString, *it++);
    ASSERT_EQ(it, mb.cend());
}

TEST(manx_buffer_test, string_elements_empty_special_default_value) {
    static const std::string defaultValue = "empty";
    ManxBuffer<std::string, 3> mb{defaultValue};
    static const std::string someString = "One";
    mb.add(someString);
    auto it = mb.cbegin();
    while (it != mb.cend() && *it == defaultValue) {
        ++it;
    }
    ASSERT_EQ(someString, *it++);
    ASSERT_EQ(it, mb.cend());
}

TEST(manx_buffer_test, copy_construction) {
    ManxBuffer<int, 3> rb1;
    rb1.add(1);
    rb1.add(2);
    rb1.add(3);
    rb1.add(4);
    ManxBuffer<int, 3> rb2(rb1);
    auto it1 = rb1.begin();
    auto it2 = rb2.begin();
    while (it1 != rb1.end()) {
        ASSERT_EQ(*it2++, *it1++);
    }
    ASSERT_EQ(rb1.end(), it1);
    ASSERT_EQ(rb2.end(), it2);
}

} // namespace manx_buffer_test
} // namespace testing
