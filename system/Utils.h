#ifndef UTILS_H
#define	UTILS_H

#include <string>
#include <sstream>

class Utils {
public:
	static void replaceAll(std::string& str, const std::string& from, const std::string& to);
	template<class T>
	static std::string t_to_string(T i)
	{
		std::stringstream ss;
		std::string s;
		ss << i;
		s = ss.str();

		return s;
	}
};

#endif