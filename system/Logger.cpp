#include "Logger.h"
#include "Utils.h"

Logger::Logger()
{
	logger_type.assign("Generic logger");
	// set defautl options
	fname = "/dev/stdout";
	ss.setf(ios::fixed,ios::floatfield);
	ss.precision(10);
	available = false;
}

Logger::Logger(Logger &logger)
{
	this->proc_num = logger.proc_num;
	this->ss.flags(logger.ss.flags());
	this->ss.precision(logger.ss.precision());
	this->ss << logger.ss.str();
	this->outs = logger.outs;
	this->outbuff << logger.outbuff.str();
	this->available = logger.outbuff;
}

Logger::~Logger()
{
	if (available)
		outs->close();
}

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

Logger* Logger::getInstace()
{
	static Logger l;
	return &l;
}
void Logger::setFileOutput(string fname)
{
	logger_type.assign("Generic logger (file output)");
	this->fname = fname;
}

void Logger::init()
{
	Utils::replaceAll(fname, "%p", Utils::t_to_string(proc_num));
	outs = new ofstream(fname.c_str(), fstream::app);	
	available = true;
}