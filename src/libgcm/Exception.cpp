#include "Exception.h"

std::string gcm::Exception::demangle(const char* symbol) {
  size_t size;
  int status;
  char temp[128];
  char* demangled;
  //first, try to demangle a c++ name
  if (1 == sscanf(symbol, "%*[^(]%*[^_]%127[^)+]", temp)) {
    if (NULL != (demangled = abi::__cxa_demangle(temp, NULL, &size, &status))) {
      std::string result(demangled);
      free(demangled);
      return result;
    }
  }
  //if that didn't work, try to get a regular c symbol
  if (1 == sscanf(symbol, "%127s", temp)) {
    return temp;
  }

  //if all else fails, just return the symbol
  return symbol;
}
void gcm::Exception::saveCallStack() {
    void** buffer = new void*[100];
    callStackLen = backtrace(buffer, 100);
    callStack = backtrace_symbols(buffer, callStackLen);
    delete[] buffer;
}

gcm::Exception::Exception() {
    saveCallStack();
    this->code  = Exception::UNKNOWN;
    this->line = -1;
    this->message = "";
    this->file = "";
}

string gcm::Exception::getCallStack() {
    stringstream ss;
    for (int i = 0; i < callStackLen; i++)
        ss << demangle(callStack[i]) << "\n";
    return ss.str();
}

gcm::Exception::Exception(int code, string message, string file, int line) {
    saveCallStack();
    this->code = code;
    this->line = line;
    this->message = message;
    this->file = file;
}

gcm::Exception::~Exception() {
    free(callStack);
}
int gcm::Exception::getCode() {
    return code;
}

string gcm::Exception::getMessage() {
    return message;
}

string gcm::Exception::getFile() {
    return file;
}

int gcm::Exception::getLine() {
    return line;
}