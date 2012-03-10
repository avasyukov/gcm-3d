#ifndef _GCM_LOGGER_H
#define _GCM_LOGGER_H  1

#include <string>
#include <vector>
#include <iostream>

using std::string;
using std::vector;
using std::cout;
using std::endl;

class Logger
{
public:
	Logger();
	~Logger();
	void write(string str);
	// stores process number to make logging more verbose
	void set_proc_num(int proc_num);
	string* get_logger_type();
protected:
	string logger_type;
	int proc_num;
};

#endif
