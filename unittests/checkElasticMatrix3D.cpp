#include "../datatypes/ElasticMatrix3D.h"

int main()
{
	ElasticMatrix3D* simple = new ElasticMatrix3D();
	ElasticMatrix3D* advanced = new ElasticMatrix3D();

	log->write(string("Testing ElasticMatrix3D calculation:"));

	// Check X
	simple->prepare_matrix(1000, 1000, 10, 0, log);
	advanced->prepare_matrix(1000, 1000, 10, 1, 0, 0, log);

	if(simple->A != advanced->A)
		log->write(string("\tCross-check x\t- FAILED"));
	else
		log->write(string("\tCross-check x\t- OK"));

	if(simple->self_check(log) < 0)
		log->write(string("\tSimple Ax\t- FAILED"));
	else
		log->write(string("\tSimple Ax\t- OK"));

	if(advanced->self_check(log) < 0)
		log->write(string("\tAdvanced Ax\t- FAILED"));
	else
		log->write(string("\tAdvanced Ax\t- OK"));


	// Check Y
	simple->prepare_matrix(1000, 1000, 10, 1, log);
	advanced->prepare_matrix(1000, 1000, 10, 0, 1, 0, log);

	if(simple->A != advanced->A)
		log->write(string("\tCross-check y\t- FAILED"));
	else
		log->write(string("\tCross-check y\t- OK"));

	if(simple->self_check(log) < 0)
		log->write(string("\tSimple Ay\t- FAILED"));
	else
		log->write(string("\tSimple Ay\t- OK"));

	if(advanced->self_check(log) < 0)
		log->write(string("\tAdvanced Ay\t- FAILED"));
	else
		log->write(string("\tAdvanced Ay\t- OK"));


	// Check Z
	simple->prepare_matrix(1000, 1000, 10, 2, log);
	advanced->prepare_matrix(1000, 1000, 10, 0, 0, 1, log);

	if(simple->A != advanced->A)
		log->write(string("\tCross-check z\t- FAILED"));
	else
		log->write(string("\tCross-check z\t- OK"));

	if(simple->self_check(log) < 0)
		log->write(string("\tSimple Az\t- FAILED"));
	else
		log->write(string("\tSimple Az\t- OK"));

	if(advanced->self_check(log) < 0)
		log->write(string("\tAdvanced Az\t- FAILED"));
	else
		log->write(string("\tAdvanced Az\t- OK"));


	//Check random
	advanced->prepare_matrix(1000, 1000, 10, 1, 1, 1, log);
	if(advanced->self_check(log) < 0)
		log->write(string("\tAdvanced random\t- FAILED"));
	else
		log->write(string("\tAdvanced random\t- OK"));

	log->write(string("Done"));

	return 0;
};
