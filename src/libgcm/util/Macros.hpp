#ifndef MACROS_HPP
#define MACROS_HPP 

#define DO_ONCE(statement) do { statement } while(0)
#define GET_MACRO_2(_1, _2, MACRO_NAME, ...) MACRO_NAME
#define GET_MACRO_3(_1, _2, _3, MACRO_NAME, ...) MACRO_NAME

#endif /* MACROS_HPP */
