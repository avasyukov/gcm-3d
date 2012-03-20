#ifndef _GCM_LOGGER_H
#define _GCM_LOGGER_H  1

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <fstream>

//using std::string;
//using std::vector;
//using std::cout;
//using std::endl;
//using std::stringstream;
//using std::ios;
//
using namespace std;

class Logger
{
public:
	Logger();
	Logger(string filename);
	Logger(Logger &logger);
	~Logger();
	void write(string str);
	// stores process number to make logging more verbose
	void set_proc_num(int proc_num);
	// returns stringstream
	stringstream &get_ss();
	string* get_logger_type();
	// FIXME
	// it seems to be impossible to move this template operator outside of a
	// header file so it's declared here
	template<class T>
	Logger &operator<<(T val)
	{
		ss << val;
		return *this;
	}
	template<class T>
	Logger &operator<(T val)
	{
		ss << val;
		write(ss.str());
		ss.str(std::string());
		return *this;
	}
protected:
	ofstream *outs;
	string logger_type;
	stringstream ss;
	int proc_num;
};

#endif
