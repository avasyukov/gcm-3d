#ifndef GCM_LOGGING_H_
#define GCM_LOGGING_H_

#include "libgcm/util/Macros.hpp"
#include "libgcm/config.hpp"
#include <iostream>
using std::cout;
using std::endl;

#if CONFIG_ENABLE_LOGGING
    #include <log4cxx/logger.h>
    #include <log4cxx/mdc.h>
    #define USE_LOGGER log4cxx::LoggerPtr logger;
    #define INIT_LOGGER(name) logger = log4cxx::Logger::getLogger(name)
    #define USE_AND_INIT_LOGGER(name) log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger(name);
#if CONFIG_ENABLE_LOGGING_TRACE
    #define TRACE_ON_EXCEPTION(statement) DO_ONCE( \
        try { \
            DO_ONCE(statement;); \
        } catch (...) { \
            log4cxx::Logger::getRootLogger()->setLevel(log4cxx::Level::getTrace()); \
            DO_ONCE(statement;); \
            throw; \
        } \
    )
#else
    #define TRACE_ON_EXCEPTION(statement) DO_ONCE(statement;);
#endif
#else
    #define USE_LOGGER ;
    #define USE_AND_INIT_LOGGER(name) ;
    #define INIT_LOGGER(name) ;
    #define USE_AND_INIT_LOGGER(name) ;
    #define TRACE_ON_EXCEPTION(statement) ;
#endif


#if CONFIG_ENABLE_LOGGING_INFO
    #if CONFIG_ENABLE_LOGGING
        #define LOG_INFO(message)  DO_ONCE(LOG4CXX_INFO (logger, message))
    #else
        #define LOG_INFO(message)  DO_ONCE(cout << message << endl;)
    #endif
#else
    #define LOG_INFO(message)  DO_ONCE(;)
#endif

#if CONFIG_ENABLE_LOGGING_WARN
    #if CONFIG_ENABLE_LOGGING
        #define LOG_WARN(message)  DO_ONCE(LOG4CXX_WARN (logger, message))
    #else
        #define LOG_WARN(message)  DO_ONCE(cout << message << endl;)
    #endif
#else
    #define LOG_WARN(message)  DO_ONCE(;)
#endif

#if CONFIG_ENABLE_LOGGING_FATAL
    #if CONFIG_ENABLE_LOGGING
        #define LOG_FATAL(message)  DO_ONCE(LOG4CXX_FATAL (logger, message))
    #else
        #define LOG_FATAL(message)  DO_ONCE(cout << message << endl;)
    #endif
#else
    #define LOG_FATAL(message)  DO_ONCE(;)
#endif

#if CONFIG_ENABLE_LOGGING_ERROR
    #if CONFIG_ENABLE_LOGGING
        #define LOG_ERROR(message)  DO_ONCE(LOG4CXX_ERROR (logger, message))
    #else
        #define LOG_ERROR(message)  DO_ONCE(cout << message << endl;)
    #endif
#else
    #define LOG_ERROR(message)  DO_ONCE(;)
#endif

#if CONFIG_ENABLE_LOGGING_TRACE
    #if CONFIG_ENABLE_LOGGING
        #define LOG_TRACE(message)  DO_ONCE(LOG4CXX_TRACE (logger, message))
    #else
        #define LOG_TRACE(message)  DO_ONCE(cout << message << endl;)
    #endif
#else
    #define LOG_TRACE(message)  DO_ONCE(;)
#endif

#if CONFIG_ENABLE_LOGGING_DEBUG
    #if CONFIG_ENABLE_LOGGING
        #define LOG_DEBUG(message)  DO_ONCE(LOG4CXX_DEBUG (logger, message))
    #else
        #define LOG_DEBUG(message)  DO_ONCE(cout << message << endl;)
    #endif
#else
    #define LOG_DEBUG(message)  DO_ONCE(;)
#endif

#endif
