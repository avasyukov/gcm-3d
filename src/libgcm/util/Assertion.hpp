#ifndef ASSERTION_HPP
#define ASSERTION_HPP

#include <string>

#include "libgcm/Exception.hpp"
#include "libgcm/util/Macros.hpp"
#include "libgcm/config.hpp"

namespace std
{
    inline std::string to_string(const std::string& s)
    {
        return s;
    }
}

#if CONFIG_ENABLE_ASSERTIONS
#define THROW_ASSERTION_FAILED(msg) THROW(gcm::Exception::UNKNOWN, msg)
#else
#define THROW_ASSERTION_FAILED(msg) DO_ONCE(;)
#endif

// assert_true
#define __assert_true_debug(cond, debug_code) DO_ONCE( \
    if (!(cond)) \
	{ \
		DO_ONCE(debug_code); \
        THROW_ASSERTION_FAILED("Condition evaluated to false instead of true. Condition: " #cond); \
	} \
)
#define __assert_true_no_debug(cond) __assert_true_debug(cond, {})
#define assert_true(...) GET_MACRO_2(__VA_ARGS__, __assert_true_debug, __assert_true_no_debug)(__VA_ARGS__)
// assert_false
#define __assert_false_debug(cond, debug_code) DO_ONCE( \
    if (cond) \
	{ \
    	DO_ONCE(debug_code); \
        THROW_ASSERTION_FAILED("Condition evaluated to true instead of false. Condition: " #cond); \
	} \
)
#define __assert_false_no_debug(cond) __assert_false_debug(cond, {})
#define assert_false(...) GET_MACRO_2(__VA_ARGS__, __assert_false_debug, __assert_false_no_debug)(__VA_ARGS__)
// assert_ne
#define __assert_ne_debug(v1, v2, debug_code) DO_ONCE( \
    if ((v1) == (v2)) \
	{ \
    	DO_ONCE(debug_code); \
        THROW_ASSERTION_FAILED(std::string("Values " #v1 " and " #v2 " expected not to be equal, but both evaluate to ") + std::to_string(v1) + std::string(".")); \
	} \
)
#define __assert_ne_no_debug(v1, v2) __assert_ne_debug(v1, v2, {})
#define assert_ne(v1, ...) GET_MACRO_3(v1, __VA_ARGS__, __assert_ne_debug, __assert_ne_no_debug)(v1, __VA_ARGS__)
// assert_eq
#define __assert_eq_debug(v1, v2, debug_code) DO_ONCE( \
    if ((v1) != (v2)) \
	{ \
    	DO_ONCE(debug_code); \
        THROW_ASSERTION_FAILED(std::string("Values " #v1 " and " #v2 " expected to be equal, but " #v1 " evaluates to ") + std::to_string(v1) + std::string(" and " #v2 " evaluates to ") + std::to_string(v2) + std::string(".")); \
	} \
)
#define __assert_eq_no_debug(v1, v2) __assert_eq_debug(v1, v2, {})
#define assert_eq(v1, ...) GET_MACRO_3(v1, __VA_ARGS__, __assert_eq_debug, __assert_eq_no_debug)(v1, __VA_ARGS__)
// assert_gt
#define __assert_gt_debug(v1, v2, debug_code) DO_ONCE( \
    if ((v1) <= (v2)) \
	{ \
    	DO_ONCE(debug_code); \
        THROW_ASSERTION_FAILED(std::string("Value " #v1 " expected to be greater than " #v2 ", but " #v1 " evaluates to ") + std::to_string(v1) + std::string(" and " #v2 " evaluates to ") + std::to_string(v2) + std::string(".")); \
	} \
)
#define __assert_gt_no_debug(v1, v2) __assert_gt_debug(v1, v2, {})
#define assert_gt(v1, ...) GET_MACRO_3(v1, __VA_ARGS__, __assert_gt_debug, __assert_gt_no_debug)(v1, __VA_ARGS__)
// assert_ge
#define __assert_ge_debug(v1, v2, debug_code) DO_ONCE( \
    if ((v1) < (v2)) \
	{ \
    	DO_ONCE(debug_code); \
        THROW_ASSERTION_FAILED(std::string("Value " #v1 " expected to be greater than (or equal to) " #v2 ", but " #v1 " evaluates to ") + std::to_string(v1) + std::string(" and " #v2 " evaluates to ") + std::to_string(v2) + std::string(".")); \
	} \
)
#define __assert_ge_no_debug(v1, v2) __assert_ge_debug(v1, v2, {})
#define assert_ge(v1, ...) GET_MACRO_3(v1, __VA_ARGS__, __assert_ge_debug, __assert_ge_no_debug)(v1, __VA_ARGS__)
// assert_lt
#define __assert_lt_debug(v1, v2, debug_code) DO_ONCE( \
    if ((v1) >= (v2)) \
	{ \
    	DO_ONCE(debug_code); \
        THROW_ASSERTION_FAILED(std::string("Value " #v1 " expected to be less than " #v2 ", but " #v1 " evaluates to ") + std::to_string(v1) + std::string(" and " #v2 " evaluates to ") + std::to_string(v2) + std::string(".")); \
	} \
)
#define __assert_lt_no_debug(v1, v2) __assert_lt_debug(v1, v2, {})
#define assert_lt(v1, ...) GET_MACRO_3(v1, __VA_ARGS__, __assert_lt_debug, __assert_lt_no_debug)(v1, __VA_ARGS__)
// assert_le
#define __assert_le_debug(v1, v2, debug_code) DO_ONCE( \
    if ((v1) > (v2)) \
	{ \
    	DO_ONCE(debug_code); \
        THROW_ASSERTION_FAILED(std::string("Value " #v1 " expected to be less than (or equal to) " #v2 ", but " #v1 " evaluates to ") + std::to_string(v1) + std::string(" and " #v2 " evaluates to ") + std::to_string(v2) + std::string(".")); \
	} \
)
#define __assert_le_no_debug(v1, v2) __assert_le_debug(v1, v2, {})
#define assert_le(v1, ...) GET_MACRO_3(v1, __VA_ARGS__, __assert_le_debug, __assert_le_no_debug)(v1, __VA_ARGS__)

#endif /* ASSERTION_HPP */
