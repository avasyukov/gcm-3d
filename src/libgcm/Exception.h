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
    /*
     * Base exception class
     */
    class Exception {
        private:
            /*
             * Exception details: code, message, file and line number.
             */
            int code;
            int line;
            string message;
            string file;
            /*
             * Call stack.
             */
            char **callStack;
            int callStackLen;
            /*
             * Saves current call stack.
             */
            void saveCallStack();
            /*
             * Demangles symbol.
             */
            string demangle(const char* symbol);
        public:
            /*
             * Constructors and destructors.
             */
            Exception();
            ~Exception();
            Exception(int code, string message, string file, int line);
            /*
             * Returns exception code.
             */
            int getCode();
            /*
             * Returns exception message.
             */
            string getMessage();
            /*
             * Returns name of the file where exception was thrown.
             */
            string getFile();
            /*
             * Returns number of the file where exception was thrown.
             */
            int getLine();
            /*
             * Returns all call stack as a single string.
             */
            string getCallStack();
            /*
             * Constants
             */
            static const int UNSUPPORTED   = -1;
            static const int UNKNOWN       = 0;
            static const int INVALID_ARG   = 1;
            static const int INVALID_INPUT = 2;
            static const int INVALID_OP    = 3;
            static const int BAD_MESH      = 4;
            static const int BAD_CONFIG    = 5;
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
