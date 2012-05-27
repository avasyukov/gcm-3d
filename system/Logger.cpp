#include "Logger.h"

Logger::Logger()
{
	logger_type.assign("Generic logger");
	// set defautl options
	fname = "/dev/stdout";
	ss.setf(ios::fixed,ios::floatfield);
	ss.precision(10);
}

Logger::Logger(string filename)
{
	logger_type.assign("Generic logger (file output)");
	// set defautl options
	ss.setf(ios::fixed,ios::floatfield);
	ss.precision(10);

	fname = filename;
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
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%d/%m/%Y %X", &tstruct);

	*outs << buf << " PE #" << proc_num << ": " << str << endl;
};

string* Logger::get_logger_type()
{
	return &logger_type;
}

void Logger::set_proc_num(int proc_num)
{
	this->proc_num = proc_num;
	char filename[1000];
	sprintf(filename, fname.c_str(), proc_num);
	outs = new ofstream(filename, fstream::app);	
}
stringstream &Logger::get_ss()
{
	return ss;
}
