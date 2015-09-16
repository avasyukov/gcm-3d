#ifndef GCM_LOGGING_H_
#define GCM_LOGGING_H_

#include "libgcm/util/Macros.hpp"
#include "libgcm/config.hpp"

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
    #define LOG_INFO(message)  DO_ONCE(LOG4CXX_INFO (logger, message))
#else
    #define LOG_INFO(message)  DO_ONCE(;)
#endif

#if CONFIG_ENABLE_LOGGING_WARN
    #define LOG_WARN(message)  DO_ONCE(LOG4CXX_WARN (logger, message))
#else
    #define LOG_WARN(message)  DO_ONCE(;)
#endif

#if CONFIG_ENABLE_LOGGING_FATAL
    #define LOG_FATAL(message)  DO_ONCE(LOG4CXX_FATAL (logger, message))
#else
    #define LOG_FATAL(message)  DO_ONCE(;)
#endif

#if CONFIG_ENABLE_LOGGING_ERROR
    #define LOG_ERROR(message)  DO_ONCE(LOG4CXX_ERROR (logger, message))
#else
    #define LOG_ERROR(message)  DO_ONCE(;)
#endif

#if CONFIG_ENABLE_LOGGING_TRACE
    #define LOG_TRACE(message)  DO_ONCE(LOG4CXX_TRACE (logger, message))
#else
    #define LOG_TRACE(message)  DO_ONCE(;)
#endif

#if CONFIG_ENABLE_LOGGING_DEBUG
    #define LOG_DEBUG(message)  DO_ONCE(LOG4CXX_DEBUG (logger, message))
#else
    #define LOG_DEBUG(message)  DO_ONCE(;)
#endif

#endif
