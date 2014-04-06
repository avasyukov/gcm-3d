#include <gtest/gtest.h>

#include "libgcm/util/Assertion.hpp"
#include "libgcm/Exception.hpp"

using namespace gcm;

#ifndef CONFIG_WITHOUT_ASSERTIONS
#define FAILED(statement) ASSERT_THROW(statement, Exception)
#define PASSED(statement) ASSERT_NO_THROW(statement)
#else
#define FAILED(statement) ASSERT_NO_THROW(statement)
#define PASSED(statement) ASSERT_NO_THROW(statement)
#endif


TEST(Assertions, AssertTrue) {
    PASSED(
        assert_true(1+1 > 1+0)
    );

    FAILED(
        assert_true(1+1 > 1+2)
    );
}

TEST(Assertions, AssertFalse) {
    PASSED(
        assert_false(1+1 < 1+0)
    );

    FAILED(
        assert_false(1+1 < 1+2)
    );
}

TEST(Assertions, AssertNE) {
    PASSED(
        assert_ne(1+1, 1+2)
    );

    FAILED(
        assert_ne(1+3, 2+2)
    );
}

TEST(Assertions, AssertEq) {
    PASSED(
        assert_eq(1+1, 1+1)
    );

    FAILED(
        assert_eq(1+3, 2+3)
    );
}

TEST(Assertions, AssertGT) {
    PASSED(
        assert_gt(1+1, 1+0)
    );

    FAILED(
        assert_gt(1+3, 2+2)
    );
    
    FAILED(
        assert_gt(1+3, 2+4)
    );
}

TEST(Assertions, AssertGE) {
    PASSED(
        assert_ge(1+1, 1+0)
    );
    
    PASSED(
        assert_ge(1+1, 1+1)
    );

    FAILED(
        assert_ge(1+3, 2+3)
    );
}

TEST(Assertions, AssertLT) {
    PASSED(
        assert_lt(1+0, 1+1)
    );

    FAILED(
        assert_lt(2+2, 1+3)
    );
    
    FAILED(
        assert_lt(2+4, 1+3)
    );
}

TEST(Assertions, AssertLE) {
    PASSED(
        assert_le(1+0, 1+1)
    );
    
    PASSED(
        assert_le(1+1, 1+1)
    );

    FAILED(
        assert_le(2+3, 1+3)
    );
}
