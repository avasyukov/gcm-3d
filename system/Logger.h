#ifndef _GCM_LOGGER_H
#define _GCM_LOGGER_H  1

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

class Logger
{
public:
	static Logger* getInstace();
	void setFileOutput(string fname);
	void init();
	// stores process number to make logging more verbose
	void set_proc_num(int proc_num);
	// returns stringstream
	stringstream &get_ss();
	string* get_logger_type();
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
		time_t now = time(0);
		struct tm tstruct;
		char buf[80];
		tstruct = *localtime(&now);
		strftime(buf, sizeof(buf), "%d/%m/%Y %X", &tstruct);

		outbuff  << buf << " PE #" << proc_num << ": " << ss.str() << endl;					
		ss.str(std::string());
		if (available)
		{
			*outs << outbuff.str();
			outs->flush();
			outbuff.str(std::string());
		}

		return *this;
	}
protected:
	Logger();
	Logger(string filename);
	Logger(Logger &logger);
	~Logger();
	void write(string str);	
	ofstream *outs;
	string logger_type;
	stringstream ss;
	string fname;
	int proc_num;
	bool available;
	stringstream outbuff;
};

#endif