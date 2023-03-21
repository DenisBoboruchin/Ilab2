#include <gtest/gtest.h>

#include "vector.h"
#include <cstddef>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#define TEST_STD_VECTOR

#ifdef TEST_STD_VECTOR
using std::vector;
#else
using my_containers::vector;
#endif

TEST(vector, Default_initialize) {
    vector<std::string> x;
    ASSERT_TRUE(x.empty());
    ASSERT_TRUE(x.size() == 0);
    ASSERT_TRUE(x.capacity() == 0);
    try
    {
        x.pop_back();
    }
    catch (std::out_of_range const& e)
    {
        std::cout << e.what() << '\n';
    }
    ASSERT_THROW(x.at(0), std::out_of_range);
    ASSERT_THROW(x.at(1), std::out_of_range);
}

TEST(vector, Default_copy_initialize) {
    vector<std::string> x = {};
    ASSERT_TRUE(x.empty());
    ASSERT_TRUE(x.size() == 0);
    ASSERT_TRUE(x.capacity() == 0);
    ASSERT_THROW(x.at(0), std::out_of_range);
    ASSERT_THROW(x.at(1), std::out_of_range);
}

TEST(vector, Constructor_from_size_t_is_explicit) {
    bool is = std::is_constructible_v<vector<std::string>, std::size_t>;
    ASSERT_TRUE(is);
    is = std::is_convertible_v<std::size_t, vector<std::string>>;
    ASSERT_TRUE(!is);
}

TEST(vector, Constructor_from_is_implicit) {
    // For compatibility with std::vector<>.
    vector<std::string> vec = {5, std::string("hi")};
    ASSERT_TRUE(vec.size() == 5);
}

struct MinimalObj {
    int id;  // NOLINT(misc-non-private-member-variables-in-classes)

    // Draft check for leaks, double-frees and non-inits.
    // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
    std::string data = std::string(500U, 'x');

    explicit MinimalObj(int id_) : id(id_) {}
    MinimalObj(MinimalObj &&) = default;
    MinimalObj &operator=(MinimalObj &&) = default;

    MinimalObj(const MinimalObj &) = delete;
    MinimalObj &operator=(const MinimalObj &) = delete;

    ~MinimalObj() = default;
};

struct ObjWithDefaultCtor : MinimalObj {
    using MinimalObj::MinimalObj;

    explicit ObjWithDefaultCtor() : MinimalObj(100) {
    }
};
static_assert(std::is_default_constructible_v<ObjWithDefaultCtor>);

struct ObjWithCopyCtor : MinimalObj {
    using MinimalObj::MinimalObj;

    ObjWithCopyCtor(const ObjWithCopyCtor &other) : MinimalObj(other.id) {
    }
    ObjWithCopyCtor &operator=(const ObjWithCopyCtor &) = delete;

    ObjWithCopyCtor(ObjWithCopyCtor &&) = default;
    ObjWithCopyCtor &operator=(ObjWithCopyCtor &&) = default;

    ~ObjWithCopyCtor() = default;
};

struct ObjWithCopyAssignment : MinimalObj {
    using MinimalObj::MinimalObj;

    ObjWithCopyAssignment(const ObjWithCopyAssignment &other)
        : MinimalObj(other.id) {
    }
    ObjWithCopyAssignment &operator=(const ObjWithCopyAssignment &other) {
        id = other.id;
        return *this;
    }

    ObjWithCopyAssignment(ObjWithCopyAssignment &&) = default;
    ObjWithCopyAssignment &operator=(ObjWithCopyAssignment &&) = default;

    ~ObjWithCopyAssignment() = default;
};

TEST(vector, Construct_empty) {

    vector<MinimalObj> v;

    ASSERT_TRUE(v.empty());

    vector<MinimalObj> vv = {};

    ASSERT_TRUE(vv.empty());

}

TEST(vector, Construct_zero_elements) {
    vector<ObjWithDefaultCtor> v(0);
    ASSERT_TRUE(v.empty());
    ASSERT_TRUE(v.size() == 0);
#ifndef TEST_STD_VECTOR
    ASSERT_TRUE(v.capacity() == 0);
#endif
}

TEST(vector, Construct_n_elements_and_read) {
    vector<ObjWithDefaultCtor> v(5);

    ASSERT_TRUE(!v.empty());
    ASSERT_TRUE(v.size() == 5);
#ifndef TEST_STD_VECTOR
    ASSERT_TRUE(v.capacity() == 8);
#endif

    ASSERT_TRUE(v[0].id == 100);
    ASSERT_TRUE(v[1].id == 100);
    ASSERT_TRUE(v[2].id == 100);
    ASSERT_TRUE(v[3].id == 100);
    ASSERT_TRUE(v[4].id == 100);
}

TEST(vector, Construct_n_copies_elements_and_read) {
    const ObjWithCopyCtor obj(10);
    vector<ObjWithCopyCtor> v(5, obj);

    ASSERT_TRUE(!v.empty());
    ASSERT_TRUE(v.size() == 5);
#ifndef TEST_STD_VECTOR
    ASSERT_TRUE(v.capacity() == 8);
#endif

    ASSERT_TRUE(v[0].id == 10);
    ASSERT_TRUE(v[1].id == 10);
    ASSERT_TRUE(v[2].id == 10);
    ASSERT_TRUE(v[3].id == 10);
    ASSERT_TRUE(v[4].id == 10);
}

TEST(vector, push_back_moves) {
    vector<MinimalObj> v;
    v.push_back(MinimalObj(10));
    v.push_back(MinimalObj(11));
    v.push_back(MinimalObj(12));
    v.push_back(MinimalObj(13));
    v.push_back(MinimalObj(14));

    ASSERT_TRUE(!v.empty());
    ASSERT_TRUE(v.size() == 5);
#ifndef TEST_STD_VECTOR
    ASSERT_TRUE(v.capacity() == 8);
#endif
    ASSERT_TRUE(v[0].id == 10);
    ASSERT_TRUE(v[1].id == 11);
    ASSERT_TRUE(v[2].id == 12);
    ASSERT_TRUE(v[3].id == 13);
    ASSERT_TRUE(v[4].id == 14);
}

TEST(vector, push_back_copies) {
    vector<ObjWithCopyCtor> v;
    const ObjWithCopyCtor obj(10);
    v.push_back(obj);
    v.push_back(obj);
    v.push_back(obj);

    ASSERT_TRUE(!v.empty());
    ASSERT_TRUE(v.size() == 3);
#ifndef TEST_STD_VECTOR
    ASSERT_TRUE(v.capacity() == 4);
#endif
    ASSERT_TRUE(v[0].id == 10);
    ASSERT_TRUE(v[1].id == 10);
    ASSERT_TRUE(v[2].id == 10);
}

TEST(vector, Copy_construct) {
    vector<ObjWithCopyCtor> orig;
    orig.push_back(ObjWithCopyCtor(10));
    orig.push_back(ObjWithCopyCtor(11));
    orig.push_back(ObjWithCopyCtor(12));
    orig.push_back(ObjWithCopyCtor(13));
    orig.push_back(ObjWithCopyCtor(14));
    // To check whether copy chooses minimal possible capacity.
    orig.pop_back();
    orig.pop_back();

    auto check_vec = [](const vector<ObjWithCopyCtor> &v) {
        ASSERT_TRUE(!v.empty());
        ASSERT_TRUE(v.size() == 3);
        ASSERT_TRUE(v[0].id == 10);
        ASSERT_TRUE(v[1].id == 11);
        ASSERT_TRUE(v[2].id == 12);
    };

    const auto &orig_const = orig;
    // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
    vector<ObjWithCopyCtor> v = orig_const;

    check_vec(v);

    check_vec(orig);

#ifndef TEST_STD_VECTOR
    ASSERT_TRUE(orig.capacity() == 8);
    ASSERT_TRUE(v.capacity() == 4);
#endif
}

TEST(vector, Copy_assign) {
    vector<ObjWithCopyAssignment> orig;
    orig.push_back(ObjWithCopyAssignment(10));
    orig.push_back(ObjWithCopyAssignment(11));
    orig.push_back(ObjWithCopyAssignment(12));
    orig.push_back(ObjWithCopyAssignment(13));
    orig.push_back(ObjWithCopyAssignment(14));
    // To check whether copy chooses minimal possible capacity.
    orig.pop_back();
    orig.pop_back();

    auto check_vec = [](const vector<ObjWithCopyAssignment> &v) {
        ASSERT_TRUE(!v.empty());
        ASSERT_TRUE(v.size() == 3);
        ASSERT_TRUE(v[0].id == 10);
        ASSERT_TRUE(v[1].id == 11);
        ASSERT_TRUE(v[2].id == 12);
    };

    auto test_copy_to = [&](vector<ObjWithCopyAssignment> &v) {
        const auto &orig_const = orig;
        ASSERT_TRUE(&(v = orig_const) == &v);

        check_vec(v);
        check_vec(orig);
    };

    vector<ObjWithCopyAssignment> v;
    test_copy_to(v);
#ifndef TEST_STD_VECTOR
    ASSERT_TRUE(orig.capacity() == 8);
    ASSERT_TRUE(v.capacity() == 4);
#endif

    vector<ObjWithCopyAssignment> v2(3, ObjWithCopyAssignment(20));
    test_copy_to(v2);
#ifndef TEST_STD_VECTOR
    ASSERT_TRUE(orig.capacity() == 8);
    ASSERT_TRUE(v2.capacity() == 4);
#endif

    vector<ObjWithCopyAssignment> v3(7, ObjWithCopyAssignment(20));
    test_copy_to(v3);
#ifndef TEST_STD_VECTOR
    ASSERT_TRUE(orig.capacity() == 8);

    // We have to re-create buffer to provide strong exception safety,
    // hence we choose the minimal possible capacity.
    ASSERT_TRUE(v3.capacity() == 4);
#endif

    ObjWithCopyAssignment *orig_buf = &orig[0];
    test_copy_to(orig);
    // Ensure there were no extra copies.
    ASSERT_TRUE(&orig[0] == orig_buf);
#ifndef TEST_STD_VECTOR
    ASSERT_TRUE(orig.capacity() == 8);
#endif
}

TEST(vector, Move_construct) {
    vector<MinimalObj> orig;
    orig.push_back(MinimalObj(10));
    orig.push_back(MinimalObj(11));
    orig.push_back(MinimalObj(12));
    orig.push_back(MinimalObj(13));
    orig.push_back(MinimalObj(14));

    MinimalObj *orig_buf = &orig[0];

    vector<MinimalObj> v = std::move(orig);
    ASSERT_TRUE(!v.empty());
    ASSERT_TRUE(v.size() == 5);
#ifndef TEST_STD_VECTOR
    ASSERT_TRUE(v.capacity() == 8);
#endif
    ASSERT_TRUE(v[0].id == 10);
    ASSERT_TRUE(v[1].id == 11);
    ASSERT_TRUE(v[2].id == 12);
    ASSERT_TRUE(v[3].id == 13);
    ASSERT_TRUE(v[4].id == 14);

    // Ensure there were no extra copies.
    ASSERT_TRUE(&v[0] == orig_buf);

    // NOLINTNEXTLINE(bugprone-use-after-move)
    ASSERT_TRUE(orig.empty());
    // NOLINTNEXTLINE(bugprone-use-after-move)
    ASSERT_TRUE(orig.size() == 0);
    // NOLINTNEXTLINE(bugprone-use-after-move)
    ASSERT_TRUE(orig.capacity() == 0);
}

TEST(vector, Move_assign) {
    vector<MinimalObj> orig;
    orig.push_back(MinimalObj(10));
    orig.push_back(MinimalObj(11));
    orig.push_back(MinimalObj(12));
    orig.push_back(MinimalObj(13));
    orig.push_back(MinimalObj(14));

    MinimalObj *orig_buf = &orig[0];

    auto test_move_to_with_expected_capacity =
        [&](vector<MinimalObj> &v,
            [[maybe_unused]] std::size_t expected_capacity) {
            ASSERT_TRUE(&(v = std::move(orig)) == &v);

            ASSERT_TRUE(!v.empty());
            ASSERT_TRUE(v.size() == 5);
#ifndef TEST_STD_VECTOR
            ASSERT_TRUE(v.capacity() == 8);
#endif
            ASSERT_TRUE(v[0].id == 10);
            ASSERT_TRUE(v[1].id == 11);
            ASSERT_TRUE(v[2].id == 12);
            ASSERT_TRUE(v[3].id == 13);
            ASSERT_TRUE(v[4].id == 14);

            // Ensure there were no extra copies.
            ASSERT_TRUE(&v[0] == orig_buf);

            // NOLINTNEXTLINE(bugprone-use-after-move)
            ASSERT_TRUE(orig.empty());
            // NOLINTNEXTLINE(bugprone-use-after-move)
            ASSERT_TRUE(orig.size() == 0);
#ifndef TEST_STD_VECTOR
            // NOLINTNEXTLINE(bugprone-use-after-move)
            ASSERT_TRUE(orig.capacity() == expected_capacity);
#endif
        };
/*
    vector<MinimalObj> v;
    test_move_to_with_expected_capacity(v, 0);*/

    vector<MinimalObj> v2;
    v2.push_back(MinimalObj(100));
    v2.push_back(MinimalObj(101));
    v2.push_back(MinimalObj(102));
    test_move_to_with_expected_capacity(v2, 4);

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wself-move"
#endif
    orig = std::move(orig);
#ifdef __clang__
#pragma clang diagnostic pop
#endif
        // No further checks on the state.
}

TEST(vector, Elements_are_consecutive) {
    vector<MinimalObj> v;
    v.push_back(MinimalObj(10));
    v.push_back(MinimalObj(11));
    v.push_back(MinimalObj(12));
    v.push_back(MinimalObj(13));
    v.push_back(MinimalObj(14));

    ASSERT_TRUE(&v[0] + 1 == &v[1]);
    ASSERT_TRUE(&v[0] + 2 == &v[2]);
    ASSERT_TRUE(&v[0] + 3 == &v[3]);
    ASSERT_TRUE(&v[0] + 4 == &v[4]);
}

TEST(vector, Write_to_non_const) {
    vector<MinimalObj> v;
    v.push_back(MinimalObj(10));
    v.push_back(MinimalObj(11));
    v.push_back(MinimalObj(12));
    v.push_back(MinimalObj(13));
    v.push_back(MinimalObj(14));

    v[0].id = 15;
    ASSERT_TRUE(v[0].id == 15);

    v.at(1).id = 16;
    ASSERT_TRUE(v[1].id == 16);

    ASSERT_TRUE(&v[0] == &v.at(0));
    ASSERT_TRUE(&v[1] == &v.at(1));
    ASSERT_THROW(v.at(5), std::out_of_range);
    ASSERT_THROW(v.at(1'000'000'000), std::out_of_range);
}

TEST(vector, Read_from_const) {
    vector<MinimalObj> orig;
    orig.push_back(MinimalObj(10));
    orig.push_back(MinimalObj(11));
    orig.push_back(MinimalObj(12));
    orig.push_back(MinimalObj(13));
    orig.push_back(MinimalObj(14));

    const vector<MinimalObj> &v = orig;
    ASSERT_TRUE(!v.empty());
    ASSERT_TRUE(v.size() == 5);
#ifndef TEST_STD_VECTOR
    ASSERT_TRUE(v.capacity() == 8);
#endif
    ASSERT_TRUE(v[0].id == 10);
    ASSERT_TRUE(v.at(0).id == 10);
    ASSERT_THROW(v.at(5), std::out_of_range);
    ASSERT_THROW(v.at(1'000'000'000), std::out_of_range);
}

TEST(vector, reserve) {
    vector<MinimalObj> v;
    v.push_back(MinimalObj(10));
    v.push_back(MinimalObj(11));
    v.push_back(MinimalObj(12));
    v.push_back(MinimalObj(13));
    v.push_back(MinimalObj(14));

    auto check_vec_and_capacity =
        [&v]([[maybe_unused]] std::size_t expected_capacity) {
            ASSERT_TRUE(!v.empty());
            ASSERT_TRUE(v.size() == 5);
#ifndef TEST_STD_VECTOR
            ASSERT_TRUE(v.capacity() == expected_capacity);
#endif
            ASSERT_TRUE(v[0].id == 10);
            ASSERT_TRUE(v[1].id == 11);
            ASSERT_TRUE(v[2].id == 12);
            ASSERT_TRUE(v[3].id == 13);
            ASSERT_TRUE(v[4].id == 14);
        };


    v.reserve(5);
    check_vec_and_capacity(8);  // cppcheck-suppress invalidContainer


    v.reserve(1);
    check_vec_and_capacity(8);  // cppcheck-suppress invalidContainer


    v.reserve(8);
    check_vec_and_capacity(8);  // cppcheck-suppress invalidContainer

    v.reserve(9);
    check_vec_and_capacity(16);  // cppcheck-suppress invalidContainer


    v.reserve(100);
    check_vec_and_capacity(128);  // cppcheck-suppress invalidContainer

}

TEST(vector, pop_back) {
    vector<MinimalObj> v;
    v.push_back(MinimalObj(10));
    v.push_back(MinimalObj(11));
    v.push_back(MinimalObj(12));
    v.push_back(MinimalObj(13));
    v.push_back(MinimalObj(14));
    v.pop_back();
    v.pop_back();

    ASSERT_TRUE(!v.empty());
    ASSERT_TRUE(v.size() == 3);
#ifndef TEST_STD_VECTOR
    ASSERT_TRUE(v.capacity() == 8);
#endif
    ASSERT_TRUE(v[0].id == 10);
    ASSERT_TRUE(v[1].id == 11);
    ASSERT_TRUE(v[2].id == 12);
}

TEST(vector, pop_back_with_push_back) {
    vector<MinimalObj> v;
    v.push_back(MinimalObj(10));
    v.push_back(MinimalObj(11));
    v.push_back(MinimalObj(12));
    v.pop_back();
    v.pop_back();
    v.pop_back();

    ASSERT_TRUE(v.empty());
    ASSERT_TRUE(v.size() == 0);
#ifndef TEST_STD_VECTOR
    ASSERT_TRUE(v.capacity() == 4);
#endif

    v.push_back(MinimalObj(13));

    ASSERT_TRUE(!v.empty());
    ASSERT_TRUE(v.size() == 1);
#ifndef TEST_STD_VECTOR
    ASSERT_TRUE(v.capacity() == 4);
#endif
    ASSERT_TRUE(v[0].id == 13);

    v.pop_back();

    ASSERT_TRUE(v.empty());
    ASSERT_TRUE(v.size() == 0);
#ifndef TEST_STD_VECTOR
    ASSERT_TRUE(v.capacity() == 4);
#endif
}

TEST(vector, clear) {
    vector<MinimalObj> v;
    v.push_back(MinimalObj(10));
    v.push_back(MinimalObj(11));
    v.push_back(MinimalObj(12));
    v.push_back(MinimalObj(13));
    v.push_back(MinimalObj(14));
    v.clear();

    ASSERT_TRUE(v.empty());
    ASSERT_TRUE(v.size() == 0);
#ifndef TEST_STD_VECTOR
    ASSERT_TRUE(v.capacity() == 8);
#endif
}

TEST(vector, resize_default_constructible) {
    vector<ObjWithDefaultCtor> v;
    v.push_back(ObjWithDefaultCtor(10));
    v.push_back(ObjWithDefaultCtor(11));
    v.push_back(ObjWithDefaultCtor(12));
    v.push_back(ObjWithDefaultCtor(13));
    v.push_back(ObjWithDefaultCtor(14));


    v.resize(5);

    ASSERT_TRUE(!v.empty());
    ASSERT_TRUE(v.size() == 5);
#ifndef TEST_STD_VECTOR
    ASSERT_TRUE(v.capacity() == 8);
#endif
    ASSERT_TRUE(v[0].id == 10);
    ASSERT_TRUE(v[1].id == 11);
    ASSERT_TRUE(v[2].id == 12);
    ASSERT_TRUE(v[3].id == 13);
    ASSERT_TRUE(v[4].id == 14);


    v.resize(3);

    ASSERT_TRUE(!v.empty());
    ASSERT_TRUE(v.size() == 3);
#ifndef TEST_STD_VECTOR
    ASSERT_TRUE(v.capacity() == 8);
#endif
    ASSERT_TRUE(v[0].id == 10);
    ASSERT_TRUE(v[1].id == 11);
    ASSERT_TRUE(v[2].id == 12);

    v.resize(0);

    ASSERT_TRUE(v.empty());
    ASSERT_TRUE(v.size() == 0);
#ifndef TEST_STD_VECTOR
    ASSERT_TRUE(v.capacity() == 8);
#endif
}

TEST(vector, resize_with_copy) {
#ifdef TEST_STD_VECTOR
    using Obj = ObjWithCopyAssignment;
#else
    using Obj = ObjWithCopyCtor;
#endif
    vector<Obj> v;
    v.push_back(Obj(10));
    v.push_back(Obj(11));
    v.push_back(Obj(12));
    v.push_back(Obj(13));
    v.push_back(Obj(14));

    v.resize(5, Obj(50));

    ASSERT_TRUE(!v.empty());
    ASSERT_TRUE(v.size() == 5);
#ifndef TEST_STD_VECTOR
    ASSERT_TRUE(v.capacity() == 8);
#endif
    ASSERT_TRUE(v[0].id == 10);
    ASSERT_TRUE(v[1].id == 11);
    ASSERT_TRUE(v[2].id == 12);
    ASSERT_TRUE(v[3].id == 13);
    ASSERT_TRUE(v[4].id == 14);


    v.resize(3, Obj(50));

    ASSERT_TRUE(!v.empty());
    ASSERT_TRUE(v.size() == 3);
#ifndef TEST_STD_VECTOR
    ASSERT_TRUE(v.capacity() == 8);
#endif
    ASSERT_TRUE(v[0].id == 10);
    ASSERT_TRUE(v[1].id == 11);
    ASSERT_TRUE(v[2].id == 12);


    v.resize(0, Obj(50));

    ASSERT_TRUE(v.empty());
    ASSERT_TRUE(v.size() == 0);
#ifndef TEST_STD_VECTOR
    ASSERT_TRUE(v.capacity() == 8);
#endif


    v.resize(7, Obj(50));

    ASSERT_TRUE(!v.empty());
    ASSERT_TRUE(v.size() == 7);
#ifndef TEST_STD_VECTOR
    ASSERT_TRUE(v.capacity() == 8);
#endif

    for (int i = 0; i < 7; ++i)
    {
        ASSERT_TRUE(v[i].id == 50);
    }

    v.resize(9, Obj(50));

    ASSERT_TRUE(!v.empty());
    ASSERT_TRUE(v.size() == 9);
#ifndef TEST_STD_VECTOR
    ASSERT_TRUE(v.capacity() == 16);
#endif
    for (int i = 0; i < 9; ++i)
    {
        ASSERT_TRUE(v[i].id == 50);
    }
}

TEST(vector, 
    push_back_copy_keeps_strong_exception_safety_even_for_capacity_when_reallocating) {
    struct artificial_exception {};
    struct S {
        // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
        bool can_copy;
        // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
        std::string data = std::string(500U, 'x');
        // Fun fact: until this field was here,
        // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=96188 was triggered.

        explicit S(bool can_copy_ = true) : can_copy(can_copy_) {
        }

        S(S &&) = default;
        S &operator=(S &&) = default;

        S(const S &other) : can_copy(other.can_copy) {
            if (!other.can_copy) {
                throw artificial_exception();
            }
        }

        S &operator=(const S &other) = delete;

        ~S() = default;
    };

    vector<S> v(4);
    ASSERT_TRUE(!v.empty());
    ASSERT_TRUE(v.size() == 4);
#ifndef TEST_STD_VECTOR
    ASSERT_TRUE(v.capacity() == 4);
#endif

    const S obj(false);
    ASSERT_THROW(v.push_back(obj), artificial_exception);

    ASSERT_TRUE(!v.empty());
    ASSERT_TRUE(v.size() == 4);
#ifndef TEST_STD_VECTOR
    ASSERT_TRUE(v.capacity() == 4);
#endif
    ASSERT_TRUE(v[0].data == std::string(500U, 'x'));
    ASSERT_TRUE(v[1].data == std::string(500U, 'x'));
    ASSERT_TRUE(v[2].data == std::string(500U, 'x'));
    ASSERT_TRUE(v[3].data == std::string(500U, 'x'));
}

TEST(vector, 
    resize_keeps_strong_exception_safety_even_for_capacity_when_reallocating) {
    struct artificial_exception {};
    struct S {
        // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
        std::string data = std::string(500U, 'x');

        S() {
            throw artificial_exception();
        }

        explicit S(int) {
        }
    };

    vector<S> v;
    v.push_back(S(10));
    v.push_back(S(10));
    ASSERT_TRUE(!v.empty());
    ASSERT_TRUE(v.size() == 2);
#ifndef TEST_STD_VECTOR
    ASSERT_TRUE(v.capacity() == 2);
#endif

    ASSERT_THROW(v.resize(10), artificial_exception);

    ASSERT_TRUE(!v.empty());
    ASSERT_TRUE(v.size() == 2);
#ifndef TEST_STD_VECTOR
    ASSERT_TRUE(v.capacity() == 2);
#endif
    ASSERT_TRUE(v[0].data == std::string(500U, 'x'));
    ASSERT_TRUE(v[1].data == std::string(500U, 'x'));
}

#ifndef TEST_STD_VECTOR
TEST(vector, copy_assignment_keeps_strong_exception_safety) {
    struct artificial_exception {};
    struct S {
        // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
        bool can_copy = true;
        // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
        std::string data;

        explicit S(std::string data_) : data(std::move(data_)) {
        }

        S(S &&) = default;
        S &operator=(S &&) = default;

        S(const S &other) : can_copy(other.can_copy), data(other.data) {
            if (!can_copy) {
                throw artificial_exception();
            }
        }

        S &operator=(const S &other) {
            can_copy = other.can_copy;
            data = other.data;
            if (!can_copy) {
                throw artificial_exception();
            }
            return *this;
        }

        ~S() = default;
    };

    vector<S> v1;
    vector<S> v2;
    v1.push_back(S(std::string(500U, 'a')));
    v1.push_back(S(std::string(500U, 'b')));
    v1.push_back(S(std::string(500U, 'c')));
    v2.push_back(S(std::string(500U, 'd')));
    v2.push_back(S(std::string(500U, 'e')));
    v2.push_back(S(std::string(500U, 'f')));

    v1[1].can_copy = false;
    ASSERT_THROW(v2 = v1, artificial_exception);

    ASSERT_TRUE(!v2.empty());
    ASSERT_TRUE(v2.size() == 3);
    ASSERT_TRUE(v2.capacity() == 4);
    ASSERT_TRUE(v2[0].data == std::string(500U, 'd'));
    ASSERT_TRUE(v2[1].data == std::string(500U, 'e'));
    ASSERT_TRUE(v2[2].data == std::string(500U, 'f'));
}
#endif

TEST(vector, op_and_at_have_lvalue_or_rvalue_overloads) {
    struct TracingObj {
        // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
        int kind = 0;

        TracingObj() = default;
        TracingObj(const TracingObj &) : kind(1) {
        }
        TracingObj(TracingObj &&other) noexcept : kind(2) {
            other.kind = -2;
        }
        TracingObj &operator=(const TracingObj &) {
            kind = 3;
            return *this;
        }
        TracingObj &operator=(TracingObj &&other) noexcept {
            kind = 4;
            other.kind = -4;
            return *this;
        }
        ~TracingObj() = default;
    };
    vector<TracingObj> v(3);
    v[0].kind = 10;
    v[1].kind = 20;
    v.at(2).kind = 30;

    TracingObj o = v[0];
    ASSERT_TRUE(o.kind == 1);
    ASSERT_TRUE(v[0].kind == 10);


    TracingObj o2 = v.at(0);
    ASSERT_TRUE(o2.kind == 1);
    ASSERT_TRUE(v[0].kind == 10);


#ifndef TEST_STD_VECTOR
    TracingObj o3 = std::move(v)[0];
    ASSERT_TRUE(o3.kind == 2);
    // NOLINTNEXTLINE(bugprone-use-after-move)
    ASSERT_TRUE(v[0].kind == -2);


    TracingObj o4 = std::move(v).at(0);
    ASSERT_TRUE(o4.kind == 2);
    // NOLINTNEXTLINE(bugprone-use-after-move)
    ASSERT_TRUE(v[0].kind == -2);

#endif
}

TEST(vector, new_elements_are_value_initialized) {

    for (int step = 0; step < 10; step++) {
        vector<int> vec(1000);
        for (std::size_t i = 0; i < vec.size(); i++) {
            ASSERT_TRUE(vec[i] == 0);
            vec[i] = 10;
        }
    }

    for (int step = 0; step < 10; step++) {
        vector<int> vec;
        vec.resize(500);
        for (std::size_t i = 0; i < 500; i++) {
            ASSERT_TRUE(vec[i] == 0);
            vec[i] = 10;
        }
        vec.resize(1000);
        for (std::size_t i = 500; i < 1000; i++) {
            ASSERT_TRUE(vec[i] == 0);
            vec[i] = 10;
        }
        vec.resize(0);
        vec.resize(1000);
        for (std::size_t i = 0; i < vec.size(); i++) {
            ASSERT_TRUE(vec[i] == 0);
            vec[i] = 10;
        }
    }

}

namespace {
struct Counters {
    std::size_t new_count = 0;
    std::size_t new_total_elems = 0;
    std::size_t delete_count = 0;
    std::size_t delete_total_elems = 0;
} global_counters;
}  // namespace

Counters operator-(const Counters &a, const Counters &b) {
    return Counters{a.new_count - b.new_count,
                    a.new_total_elems - b.new_total_elems,
                    a.delete_count - b.delete_count,
                    a.delete_total_elems - b.delete_total_elems};
}

template <typename Fn>
Counters with_counters(Fn fn) {
    Counters start = global_counters;
    fn();
    return global_counters - start;
}

namespace {
template <typename T>
struct CounterAllocator {
    static_assert(alignof(T) <= __STDCPP_DEFAULT_NEW_ALIGNMENT__);

    using value_type = T;

    T *allocate(std::size_t count) {
        T *result = static_cast<T *>(::operator new(count * sizeof(T)));
        global_counters.new_count++;
        global_counters.new_total_elems += count;
        return result;
    }

    void deallocate(T *ptr, std::size_t count) noexcept {
        ASSERT_TRUE(ptr != nullptr);
        ASSERT_TRUE(count > 0);
        ::operator delete(ptr);
        global_counters.delete_count++;
        global_counters.delete_total_elems += count;
    }
};
}  // namespace

TEST(vector, custom_allocator_is_used_by_vector_string) {
    Counters res = with_counters([]() {
        struct S {
            char buf[40]{};
        };
        vector<S, CounterAllocator<S>> vec_empty;
        vector<S, CounterAllocator<S>> vec(10);
        ASSERT_TRUE(vec[0].buf[0] == 0);
    });
    ASSERT_TRUE(res.new_count == 1);
    ASSERT_TRUE(res.delete_count == 1);
#ifndef TEST_STD_VECTOR
    ASSERT_TRUE(res.new_total_elems == 16);
    ASSERT_TRUE(res.delete_total_elems == 16);
#endif
}