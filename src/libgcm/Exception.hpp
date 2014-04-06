#ifndef GCM_EXCEPTION_H_
#define GCM_EXCEPTION_H_

#include <string>
#include <sstream>

#include <execinfo.h>
#include <stdlib.h>
#include <stdio.h>
#include <cxxabi.h>

using namespace std;

namespace gcm {
    /**
     * Custom gcm exception implementation.
     */
    class Exception {
        private:
            /**
             * Exception code.
             */
            int code;
            /**
             * Line number where exception was thrown.
             */
            int line;
            /**
             * Exception message
             */
            string message;
            /**
             * File name where exception was thrown.
             */
            string file;
            /**
             * Call stack as at the moment when exception was thrown.
             */
            string callStack;
        public:
            /**
             * Constructor.
             *
             * @param code Exception code.
             * @param message Exception message.
             * @param file Name of the file where exception was thrown.
             * @param line Number of line in the file where exception was thrown.
             */
            Exception(int code, const string& message, const string& file, int line);
            /**
             * Getter for exception code.
             *
             * @return Exception code.
             */
            int getCode() const;
            /**
             * Getter for exception message.
             *
             * @return Exception message.
             */
            const string& getMessage() const;
            /**
             * Getter for exception file name.
             *
             * @return File name where exception was thrown.
             */
            const string& getFile() const;
            /**
             * Getter for line number.
             *
             * @return Line number in the file where exception was thrown.
             */
            int getLine() const;
            /**
             * Getter for call stack.
             *
             * @return String representation of call stack as at the moment when exception was thrown.
             */
            const string& getCallStack() const;
            /**
             * Unsopported exception code.
             */
            static const int UNSUPPORTED   = -1;
            /**
             * Unknown exception code.
             */
            static const int UNKNOWN       = 0;
            /**
             * Invalid argument exception code;
             */
            static const int INVALID_ARG   = 1;
            /**
             * Invalid input exception code.
             */
            static const int INVALID_INPUT = 2;
            /**
             * Invalid operation exception code.
             */
            static const int INVALID_OP    = 3;
            /**
             * Bad mesh exception code.
             */
            static const int BAD_MESH      = 4;
            /**
             * Bad condfig exception code.
             *
             * FIXME Does it really need to be here? There is no configs for libgcm anymore.
             */
            static const int BAD_CONFIG    = 5;
            /**
             * Invalid method exception code.
             */
            static const int BAD_METHOD    = 6;
    };
}

#define THROW(code, msg) do { throw gcm::Exception(code, msg, __FILE__, __LINE__); } while (0)
#define THROW_INVALID_ARG(msg) THROW(gcm::Exception::INVALID_ARG, msg)
#define THROW_INVALID_INPUT(msg) THROW(gcm::Exception::INVALID_INPUT, msg)
#define THROW_INVALID_OP(msg) THROW(gcm::Exception::INVALID_OP, msg)
#define THROW_UNSUPPORTED(msg) THROW(gcm::Exception::UNSUPPORTED, msg)
#define THROW_BAD_MESH(msg) THROW(gcm::Exception::BAD_MESH, msg)
#define THROW_BAD_CONFIG(msg) THROW(gcm::Exception::BAD_CONFIG, msg)
#define THROW_BAD_METHOD(msg) THROW(gcm::Exception::BAD_METHOD, msg)

#endif
