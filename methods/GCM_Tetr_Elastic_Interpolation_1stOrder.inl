GCM_Tetr_Elastic_Interpolation_1stOrder::GCM_Tetr_Elastic_Interpolation_1stOrder()
{
	num_method_type.assign("1st order interpolation on tetr mesh");
	elastic_matrix3d[0] = new ElasticMatrix3D();
	elastic_matrix3d[1] = new ElasticMatrix3D();
	elastic_matrix3d[2] = new ElasticMatrix3D();
	U_gsl = gsl_matrix_alloc (9, 9);
	om_gsl = gsl_vector_alloc (9);
	x_gsl = gsl_vector_alloc (9);
	p_gsl = gsl_permutation_alloc (9);
};

GCM_Tetr_Elastic_Interpolation_1stOrder::~GCM_Tetr_Elastic_Interpolation_1stOrder()
{
	delete(elastic_matrix3d[0]);
	delete(elastic_matrix3d[1]);
	delete(elastic_matrix3d[2]);
	gsl_matrix_free(U_gsl);
	gsl_vector_free(om_gsl);
	gsl_vector_free(x_gsl);
	gsl_permutation_free(p_gsl);
};

int GCM_Tetr_Elastic_Interpolation_1stOrder::prepare_part_step(ElasticNode* cur_node, int stage)
{
	if(elastic_matrix3d[stage]->prepare_matrix(cur_node->la,cur_node->mu,cur_node->rho,stage,logger) < 0)
		return -1;
	return 0;
};

int GCM_Tetr_Elastic_Interpolation_1stOrder::do_next_part_step(ElasticNode* cur_node, ElasticNode* new_node, float time_step, int stage, TetrMesh* mesh)
{
	//  Prepare matrixes  A, Lambda, Omega, Omega^(-1)
	if (prepare_part_step(cur_node, stage) < 0)
		return -1;

	// Here we will store (omega = Matrix_OMEGA * u)
	float omega[9];

	// Delta x on previous time layer for all the omegas
	// 	omega_new_time_layer(x) = omega_old_time_layer(x+dx)
	float dx[9];
	for(int i = 0; i < 9; i++)
		dx[i] = - elastic_matrix3d[stage]->L(i,i) * time_step;

	// If the corresponding point on previous time layer is inner or not
	bool inner[9];

	// We will store interpolated nodes on previous time layer here
	// We now that we need five nodes for each direction (corresponding to Lambdas -C1, -C2, 0, C2, C1)
	// TODO  - We can  deal with (lambda == 0) separately
	ElasticNode previous_nodes[5];

	// This array will link omegas with corresponding interpolated nodes they should be copied from
	int ppoint_num[9];

	// Just tmp tetr pointer
	Tetrahedron* tmp_tetr;

	int count = 0;
	// For all omegas
	for(int i = 0; i < 9; i++)
	{
		// Check prevoius omegas ...
		bool already_found = false;
		for(int j = 0; j < i; j++)
		{
			// ... And try to find if we have already worked with the required point
			// on previous time layer (or at least with the point that is close enough)
			if( fabs(dx[i] - dx[j]) <= 0.01 * fabs(dx[i] + dx[j]) )
			{
				// If we have already worked with this point - just remember the number
				already_found = true;
				ppoint_num[i] = ppoint_num[j];
				inner[i] = inner[j];
			}
		}
		// If we do not have necessary point in place - ...
		if(already_found == false)
		{
			// ... Put new number ...
			ppoint_num[i] = count;

			// ... Calculate coordinates ...
			if (stage == 0) {
				previous_nodes[count].coords[0] = cur_node->coords[0] + dx[i];
				previous_nodes[count].coords[1] = cur_node->coords[1];
				previous_nodes[count].coords[2] = cur_node->coords[2];
			} else if (stage == 1) { 
				previous_nodes[count].coords[0] = cur_node->coords[0];
				previous_nodes[count].coords[1] = cur_node->coords[1] + dx[i];
				previous_nodes[count].coords[2] = cur_node->coords[2];
			} else if (stage == 2) { 
				previous_nodes[count].coords[0] = cur_node->coords[0];
				previous_nodes[count].coords[1] = cur_node->coords[1];
				previous_nodes[count].coords[2] = cur_node->coords[2] + dx[i];
			} else {
				if(logger != NULL)
					logger->write(string("Error: GCM_Tetr_Elastic_Interpolation_1stOrder::do_next_part_step - wrong stage number!"));
				return -1;
			}

			// ... Find owner tetrahedron ...
			tmp_tetr = mesh->find_owner_tetr(previous_nodes[count].coords[0], 
				previous_nodes[count].coords[1], previous_nodes[count].coords[2], cur_node);
			if( tmp_tetr != NULL )
			{
				// ... And interpolate values
				if(mesh->interpolate(&previous_nodes[count], tmp_tetr) < 0)
				{
					if(logger != NULL)
						logger->write(string("Error: GCM_Tetr_Elastic_Interpolation_1stOrder::do_next_part_step - interpolation failed!"));
					return -1;
				}
				inner[i] = true;
			} else {
				// There is no value in interpolation in this case
				//	as long as characteristic is out of region
				inner[i] = false;
			}
			count++;
		}
	}

	int outer_count = 0;

	// For all omegas
	for(int i = 0; i < 9; i++)
	{
		// If omega is 'inner' - calculate omega value
		// omega = Matrix_OMEGA * u
		// omega on new time layer is equal to omega on previous time layer along characteristic
		if(inner[i])
		{
			omega[i] = 0;
			for(int j = 0; j < 9; j++)
			{
				omega[i] += elastic_matrix3d[stage]->U(i,j) 
							* previous_nodes[ppoint_num[i]].values[j];
			}
		// For 'outer' omega - just count it
		// The values are undefined anycase
		} else {
			outer_count++;
		}
	}

	// If all the omegas are 'inner'
	// new_u = Matrix_OMEGA^(-1) * omega
	if( outer_count == 0 )
	{
		for(int i = 0; i < 9; i++)
		{
			new_node->values[i] = 0;
			for(int j = 0; j < 9; j++)
			{
				new_node->values[i] += elastic_matrix3d[stage]->U1(i,j) * omega[j];
			}
		}
	}
	// If there are 3 'outer' omegas - we should use border algorithm
	else if ( outer_count == 3 )
	{
		// GSL data for LE solving
		int s;

		// Free border algorithm
		for(int i = 0; i < 9; i++)
		{
			// If omega is 'inner' one
			if(inner[i])
			{
				// just load appropriate values into GSL containers
				gsl_vector_set(om_gsl, i, omega[i]);
				for(int j = 0; j < 9; j++)
					gsl_matrix_set(U_gsl, i, j, elastic_matrix3d[stage]->U(i,j));
			}
			// If omega is 'outer' one
			else
			{
				// omega (as right-hand part of OLE) is zero - it it free border
				gsl_vector_set(om_gsl, i, 0);
				// corresponding string in matrix is zero ...
				for(int j = 0; j < 9; j++)
					gsl_matrix_set(U_gsl, i, j, 0);
				// ... except normal and tangential stress
				// They depend on stage, so we have this switch here
				if( stage == 0 )
				{
					if ( outer_count == 3 ) {
						gsl_matrix_set(U_gsl, i, 3, 1); outer_count--;
					} else if ( outer_count == 2 ) {
						gsl_matrix_set(U_gsl, i, 4, 1); outer_count--;
					} else if ( outer_count == 1 ) {
						gsl_matrix_set(U_gsl, i, 5, 1); outer_count--;
					}
				}
				else if ( stage == 1 )
				{
					if ( outer_count == 3 ) {
						gsl_matrix_set(U_gsl, i, 4, 1); outer_count--;
					} else if ( outer_count == 2 ) {
						gsl_matrix_set(U_gsl, i, 6, 1); outer_count--;
					} else if ( outer_count == 1 ) {
						gsl_matrix_set(U_gsl, i, 7, 1); outer_count--;
					}
				}
				else if ( stage == 2 )
				{
					if ( outer_count == 3 ) {
						gsl_matrix_set(U_gsl, i, 5, 1); outer_count--;
					} else if ( outer_count == 2 ) {
						gsl_matrix_set(U_gsl, i, 7, 1); outer_count--;
					} else if ( outer_count == 1 ) {
						gsl_matrix_set(U_gsl, i, 8, 1); outer_count--;
					}
				}
				else 
				{
					if(logger != NULL)
						logger->write(string("Error: GCM_Tetr_Elastic_Interpolation_1stOrder::do_next_part_step - wrong stage number!"));
					return -1;
				}
			}
		}

		// Solve linear equations using GSL tools
		gsl_linalg_LU_decomp (U_gsl, p_gsl, &s);
		gsl_linalg_LU_solve (U_gsl, p_gsl, om_gsl, x_gsl);

		for(int j = 0; j < 9; j++)
			new_node->values[j] = gsl_vector_get(x_gsl, j);
	}
	// If there are 'outer' omegas but not 3 ones - it means smth bad happens...
	else
	{
		if(logger != NULL)
		{
			stringstream ss;
			ss << "Error: GCM_Tetr_Elastic_Interpolation_1stOrder::do_next_part_step - 'outer' values do not match. There are " << outer_count << " of them.";
			logger->write(ss.str());
		}
		return -1;
	}

	return 0;
};

int GCM_Tetr_Elastic_Interpolation_1stOrder::get_number_of_stages()
{
	return 3;
};

float GCM_Tetr_Elastic_Interpolation_1stOrder::get_max_lambda(ElasticNode* node)
{
	// TODO To think - if we can just return sqrt((la+2*mu)/rho) or we should leave to matrix calculation?

	// We check only first matrix because lambdas are equal for all matrixes.
	// (Lambdas are functions of la, mu, ro.)
	if(prepare_part_step(node, 0) < 0)
		return -1;	// We return -1 as error safely because max_lambda is always positive.

	return elastic_matrix3d[0]->max_lambda();
};
