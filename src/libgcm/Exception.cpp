#include "libgcm/Exception.hpp"

using namespace gcm;

using std::string;

const string& Exception::getCallStack() const
{
    return callStack;
}

Exception::Exception(int code, const string& message, const string& file, int line)
{
    this->code = code;
    this->line = line;
    this->message = message;
    this->file = file;

    // save call stack
    void** buffer = new void*[100];
    int callStackLen = backtrace(buffer, 100);
    char** cs = backtrace_symbols(buffer, callStackLen);
    

    auto demangle = [](char* symbol)->string
    {
        size_t size;
        int status;
        char temp[128];
        char* demangled;

        string result;

        //first, try to demangle a c++ name
        if (1 == sscanf(symbol, "%*[^(]%*[^_]%127[^)+]", temp)) {
            if ((demangled = abi::__cxa_demangle(temp, NULL, &size, &status))) {
                result = string(demangled);
                free(demangled);
            }
        } else
            if (1 == sscanf(symbol, "%127s", temp)) {
                //if that didn't work, try to get a regular c symbol
                result = string(temp);
            }
            else
                //if all else fails, just return the symbol
                result = string(symbol);
       return result;
    };

    callStack = "";
    for (int i = 1; i < callStackLen; i++)
        callStack += demangle(cs[i]) + "\n";

    delete[] buffer;
}

int Exception::getCode() const
{
    return code;
}

const string& Exception::getMessage() const
{
    return message;
}

const string& Exception::getFile() const
{
    return file;
}

int Exception::getLine() const
{
    return line;
}
