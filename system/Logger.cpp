#include "Logger.h"

Logger::Logger()
{
	logger_type.assign("Generic logger");
};

Logger::~Logger() { };

void Logger::write(string str)
{
	cout << str << endl;
};

string* Logger::get_logger_type()
{
	return &logger_type;
}
