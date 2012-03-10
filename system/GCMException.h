#ifndef _GCM_Exception_H
#define _GCM_Exception_H 1

#include <string>

using std::string;

class GCMException  {
private:
	int code;
	string message;
public:
	GCMException(int code);
	GCMException(int code, string message);
	~GCMException();
	int getCode();
	string getMessage();

	static const int MPI_EXCEPTION  = 0;
	static const int SYNC_EXCEPTION = 1;
	static const int CONFIG_EXCEPTION = 2;
	static const int METHOD_EXCEPTION = 3;
	static const int MESH_EXCEPTION = 4;
	static const int SNAP_EXCEPTION = 5;
};
#endif
