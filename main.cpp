#include <iostream>
#include "gcm-3d-mpi.h"

using std::cout;

int main()
{
	ElasticNode* en = new ElasticNode();
	Tetrahedron_1st_order* tfo = new Tetrahedron_1st_order();
	en->local_num = 1;
	tfo->local_num = 1;
	Logger* log = new Logger();
	NumericalMethod* nm = new NumericalMethod();
	nm->attach(log);
	cout << nm->get_number_of_stages() << endl;
	nm->do_next_part_step(en, en, 0.1, 1);
	RheologyCalculator* rc = new VoidRheologyCalculator();
	rc->attach(log);
	rc->do_calc(en, en);
	Mesh* mesh = new Mesh();
	mesh->attach(log);
	mesh->attach(rc);
	mesh->attach(nm);
	return 0;
}
