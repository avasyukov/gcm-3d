#ifndef ASSERTION_HPP
#define ASSERTION_HPP

#include <string>

#include "libgcm/Exception.hpp"

#define DO_WHILE(statement) do { statement } while(0)

#ifndef CONFIG_WITHOUT_ASSERTIONS
#define THROW_ASSERTION_FAILED(msg) THROW(gcm::Exception::UNKNOWN, msg)
#else
#define THROW_ASSERTION_FAILED(msg) DO_WHILE(;)
#endif


#define assert_true(cond) DO_WHILE( \
    if (!(cond)) \
        THROW_ASSERTION_FAILED("Condition evaluated to false instead of true. Condition: " #cond); \
    )

#define assert_false(cond) DO_WHILE( \
    if (cond) \
        THROW_ASSERTION_FAILED("Condition evaluated to true instead of false. Condition: " #cond); \
    )

#define assert_ne(v1, v2) DO_WHILE( \
    if ((v1) == (v2)) \
        THROW_ASSERTION_FAILED(std::string("Values " #v1 " and " #v2 " expected not to be equal, but both evaluate to ") + std::to_string(v1) + std::string(".")); \
    )

#define assert_eq(v1, v2) DO_WHILE( \
    if ((v1) != (v2)) \
        THROW_ASSERTION_FAILED(std::string("Values " #v1 " and " #v2 " expected to be equal, but " #v1 " evaluates to ") + std::to_string(v1) + std::string(" and " #v2 " evaluates to ") + std::to_string(v2) + std::string(".")); \
    )

#define assert_gt(v1, v2) DO_WHILE( \
    if ((v1) <= (v2)) \
        THROW_ASSERTION_FAILED(std::string("Value " #v1 " expected to be greater than " #v2 ", but " #v1 " evaluates to ") + std::to_string(v1) + std::string(" and " #v2 " evaluates to ") + std::to_string(v2) + std::string(".")); \
    )

#define assert_ge(v1, v2) DO_WHILE( \
    if ((v1) < (v2)) \
        THROW_ASSERTION_FAILED(std::string("Value " #v1 " expected to be greater than (or equal to) " #v2 ", but " #v1 " evaluates to ") + std::to_string(v1) + std::string(" and " #v2 " evaluates to ") + std::to_string(v2) + std::string(".")); \
    )

#define assert_lt(v1, v2) DO_WHILE( \
    if ((v1) >= (v2)) \
        THROW_ASSERTION_FAILED(std::string("Value " #v1 " expected to be less than " #v2 ", but " #v1 " evaluates to ") + std::to_string(v1) + std::string(" and " #v2 " evaluates to ") + std::to_string(v2) + std::string(".")); \
    )

#define assert_le(v1, v2) DO_WHILE( \
    if ((v1) > (v2)) \
        THROW_ASSERTION_FAILED(std::string("Value " #v1 " expected to be less than (or equal to) " #v2 ", but " #v1 " evaluates to ") + std::to_string(v1) + std::string(" and " #v2 " evaluates to ") + std::to_string(v2) + std::string(".")); \
    )

#endif /* ASSERTION_HPP */
