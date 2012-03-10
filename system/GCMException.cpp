#include "GCMException.h"

GCMException::GCMException(int code) {
	this->code = code;
}

GCMException::GCMException(int code, string message) {
	this->code = code;
	this->message = message;
}

GCMException::~GCMException() {
}

int GCMException::getCode() {
	return code;
}

string GCMException::getMessage() {
	return message;
}
