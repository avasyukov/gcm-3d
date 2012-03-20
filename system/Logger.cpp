#include "Logger.h"

Logger::Logger()
{
	logger_type.assign("Generic logger");
	// set defautl options
	outs = new ofstream("/dev/stdout", fstream::app);
	ss.setf(ios::fixed,ios::floatfield);
	ss.precision(10);
}

Logger::Logger(string filename)
{
	logger_type.assign("Generic logger (file output)");
	// set defautl options
	ss.setf(ios::fixed,ios::floatfield);
	ss.precision(10);

	outs = new ofstream(filename.c_str(), fstream::app);
}

Logger::Logger(Logger &logger)
{
	this->proc_num = logger.proc_num;
	this->ss.flags(logger.ss.flags());
	this->ss.precision(logger.ss.precision());
	this->ss << logger.ss.str();
	this->outs = logger.outs;
}

Logger::~Logger()
{
	outs->close();
}

void Logger::write(string str)
{
	*outs << "PE #" << proc_num << ": " << str << endl;
};

string* Logger::get_logger_type()
{
	return &logger_type;
}

void Logger::set_proc_num(int proc_num)
{
	this->proc_num = proc_num;
}
stringstream &Logger::get_ss()
{
	return ss;
}
