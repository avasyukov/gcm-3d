#include "Logger.h"

Logger::Logger()
{
	logger_type.assign("Generic logger");
};

Logger::Logger(Logger &logger)
{
	this->proc_num = logger.proc_num;
	this->ss << logger.ss.str();
}

Logger::~Logger() { };

void Logger::write(string str)
{
	cout << "PE #" << proc_num << ": " << str << endl;
};

string* Logger::get_logger_type()
{
	return &logger_type;
}

void Logger::set_proc_num(int proc_num)
{
	this->proc_num = proc_num;
}
