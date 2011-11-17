GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced::GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced()
{
	num_method_type.assign("1st order interpolation on tetr mesh (with plasticity) - advanced implementation");
	// TODO - do we really need 3 matrixes? May be we can use just one to store 'current stage'?
	elastic_matrix3d[0] = new ElasticMatrix3D();
	elastic_matrix3d[1] = new ElasticMatrix3D();
	elastic_matrix3d[2] = new ElasticMatrix3D();
	U_gsl = gsl_matrix_alloc (9, 9);
	om_gsl = gsl_vector_alloc (9);
	x_gsl = gsl_vector_alloc (9);
	p_gsl = gsl_permutation_alloc (9);
	random_axis = NULL;
	random_axis_inv = NULL;
	basis_quantity = 0;
};

GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced::~GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced()
{
	delete(elastic_matrix3d[0]);
	delete(elastic_matrix3d[1]);
	delete(elastic_matrix3d[2]);
	gsl_matrix_free(U_gsl);
	gsl_vector_free(om_gsl);
	gsl_vector_free(x_gsl);
	gsl_permutation_free(p_gsl);
	if(random_axis != NULL)
		free(random_axis);
	if(random_axis_inv != NULL)
		free(random_axis_inv);
};

int GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced::prepare_part_step(ElasticNode* cur_node, int stage)
{

	int node_num = cur_node->local_num;

	if(stage < 3) {
		if( elastic_matrix3d[stage]->prepare_matrix( cur_node->la, cur_node->mu, cur_node->rho, 
					random_axis_inv[node_num].ksi[0][stage], random_axis_inv[node_num].ksi[1][stage], 
										random_axis_inv[node_num].ksi[2][stage], logger) < 0) {
			return -1;
		}
	} else {
		return -1;
	}

	return 0;
};

void GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced::drop_deviator(ElasticNode* cur_node, ElasticNode* new_node)
{

	float J2 = sqrt( ( (cur_node->values[3] - cur_node->values[6]) * (cur_node->values[3] - cur_node->values[6]) 
			+ (cur_node->values[6] - cur_node->values[8]) * (cur_node->values[6] - cur_node->values[8])
			+ (cur_node->values[3] - cur_node->values[8]) * (cur_node->values[3] - cur_node->values[8])
			+ 6 * ( (cur_node->values[4]) * (cur_node->values[4]) + (cur_node->values[5]) * (cur_node->values[5])
				+ (cur_node->values[7]) * (cur_node->values[7])) ) / 6 );

	float p = (cur_node->values[3] + cur_node->values[6] + cur_node->values[8]) / 3;

	if (cur_node->yield_limit < J2)
	{
		new_node->values[3] = ((cur_node->values[3] - p) * cur_node->yield_limit / J2) + p;
		new_node->values[4] = cur_node->values[4] * cur_node->yield_limit / J2;
		new_node->values[5] = cur_node->values[5] * cur_node->yield_limit / J2;
		new_node->values[6] = ((cur_node->values[6] - p) * cur_node->yield_limit / J2) + p;
		new_node->values[7] = cur_node->values[7] * cur_node->yield_limit / J2;
		new_node->values[8] = ((cur_node->values[8] - p) * cur_node->yield_limit / J2) + p;
	}
};

int GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced::do_next_part_step(ElasticNode* cur_node, ElasticNode* new_node, float time_step, int stage, TetrMesh* mesh)
{

	if(stage == 3) {
		drop_deviator(cur_node, new_node);
		return 0;
	}

	// Moved to get_max_lambda
	// Axis randomization is done on first part-step and stored for further part-steps
	/*if(stage == 0) {
		if(create_random_axis(cur_node, mesh) < 0)
		{
			if(logger != NULL)
				logger->write(string("ERROR: GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced::do_next_part_step - failed axis randomization!"));
			return -1;
		}
	}*/

	int node_num = cur_node->local_num;

	//  Prepare matrixes  A, Lambda, Omega, Omega^(-1)
	if (prepare_part_step(cur_node, stage) < 0)
		return -1;

	// Here we will store (omega = Matrix_OMEGA * u)
	float omega[9];

	// Delta x on previous time layer for all the omegas
	// 	omega_new_time_layer(ksi) = omega_old_time_layer(ksi+dksi)
	float dksi[9];
	for(int i = 0; i < 9; i++)
		dksi[i] = - elastic_matrix3d[stage]->L(i,i) * time_step;

	// If the corresponding point on previous time layer is inner or not
	bool inner[9];

	// We will store interpolated nodes on previous time layer here
	// We know that we need five nodes for each direction (corresponding to Lambdas -C1, -C2, 0, C2, C1)
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
			if( fabs(dksi[i] - dksi[j]) <= 0.01 * fabs(dksi[i] + dksi[j]) ) // TODO - avoid magick number!
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

			previous_nodes[count] = *cur_node;

			if(previous_nodes[count].local_num != cur_node->local_num) {
				if(logger != NULL)
					logger->write(string("previous_nodes[count].local_num != cur_node->local_num!"));
				return -1;
			}

			// ... Calculate coordinates ...
			if (stage < 3) {
				for(int j = 0; j < 3; j++) {
					previous_nodes[count].coords[j] = cur_node->coords[j] 
							+ dksi[i] * random_axis[node_num].ksi[stage][j];
				}
			} else {
				if(logger != NULL)
					logger->write(string("ERROR: GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced::do_next_part_step - wrong stage number!"));
				return -1;
			}

			// ... Find owner tetrahedron ...
			tmp_tetr = mesh->find_owner_tetr(cur_node,
							dksi[i] * random_axis[node_num].ksi[stage][0],
							dksi[i] * random_axis[node_num].ksi[stage][1],
							dksi[i] * random_axis[node_num].ksi[stage][2]);
//			tmp_tetr = mesh->find_owner_tetr(previous_nodes[count].coords[0], 
//				previous_nodes[count].coords[1], previous_nodes[count].coords[2], cur_node);

//			cout << "DEBUG: " << previous_nodes[count].coords[0] << " " 
//					<< previous_nodes[count].coords[1] << " "
//					<< previous_nodes[count].coords[2] << " " << tmp_tetr << endl;

			if( tmp_tetr != NULL )
			{
				// ... And interpolate values
				if(mesh->interpolate(&previous_nodes[count], tmp_tetr) < 0)
				{
					if(logger != NULL)
						logger->write(string("ERROR: GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced::do_next_part_step - interpolation failed!"));
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

	// TODO - to think - if all omegas are 'inner' can we skip matrix calculations and just use new_u = interpolated_u ?
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
	// TODO - ... we should also use it when l*t/h > 1 and there is 1 'outer' omega
	// 		(we should add 2 more corresponding omegas to 'outer' manually
	else if ( outer_count == 3 )
	{
		// GSL data for LE solving
		int s;

		// Fixed border algorithm
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
				// ... except velocity
				if( stage < 3 )
				{
					if ( outer_count == 3 ) {
						gsl_matrix_set(U_gsl, i, 0, 1); outer_count--;
					} else if ( outer_count == 2 ) {
						gsl_matrix_set(U_gsl, i, 1, 1); outer_count--;
					} else if ( outer_count == 1 ) {
						gsl_matrix_set(U_gsl, i, 2, 1); outer_count--;
					}
				}
				else 
				{
					if(logger != NULL)
						logger->write(string("ERROR: GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced::do_next_part_step - wrong stage number!"));
					return -1;
				}
			}
		}

		// Free border algorithm
		/*for(int i = 0; i < 9; i++)
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
				// TODO - ... and they depend on direction, so we should use random axis data here.
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
						logger->write(string("ERROR: GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced::do_next_part_step - wrong stage number!"));
					return -1;
				}
			}
		}*/

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
			ss.setf(ios::fixed,ios::floatfield);
			ss.precision(10);
			ss << "ERROR: GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced::do_next_part_step - there are " << outer_count << " 'outer' characteristics." << endl;
			ss << "STAGE: " << stage << endl;
			ss << "NODE " << node_num << ": x: " << cur_node->coords[0] 
						<< " y: " << cur_node->coords[1]
						<< " z: " << cur_node->coords[2] << endl;
			ss << "NEIGH: " << (cur_node->elements)->size() << endl;
			for(int i = 0; i < (cur_node->elements)->size(); i++) {
				Tetrahedron* tmp_tetr = mesh->get_tetrahedron( (cur_node->elements)->at(i) );
				ss << "\tTetr " << tmp_tetr->local_num << " Neigh_num: " << i << endl;
				for(int j = 0; j < 4; j++) {
					ElasticNode* tmp_node = mesh->get_node( tmp_tetr->vert[j] );
					ss << "\t\tVert: " << j << " num: " << tmp_node->local_num << "\t"
							<< " x: " << tmp_node->coords[0]
							<< " y: " << tmp_node->coords[1]
							<< " z: " << tmp_node->coords[2] << endl;
				}
			}
			for(int i = 0; i < 3; i++)
				ss << "KSI[" << i << "]: x: " << random_axis[node_num].ksi[i][0]
						<< " y: " << random_axis[node_num].ksi[i][1]
						<< " z: " << random_axis[node_num].ksi[i][2] << endl;
			for(int i = 0; i < 9; i++) {
				if(inner[i]) {
					ss << "INNER OMEGA: num: " << i
						<< " val: " << elastic_matrix3d[stage]->L(i,i)
						<< " step: " << elastic_matrix3d[stage]->L(i,i) * time_step << endl;
				} else {
					ss << "OUTER OMEGA: num: " << i 
						<< " val: " << elastic_matrix3d[stage]->L(i,i)
						<< " step: " << elastic_matrix3d[stage]->L(i,i) * time_step << endl;
				}
				ss << "\t Point x: " << previous_nodes[ppoint_num[i]].coords[0]
						<< " y: " << previous_nodes[ppoint_num[i]].coords[1]
						<< " z: " << previous_nodes[ppoint_num[i]].coords[2] << endl;
			}
			logger->write(ss.str());
		}
		return -1;
	}

	return 0;
};

int GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced::get_number_of_stages()
{
	return 4;
};

float GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced::get_max_lambda(ElasticNode* node, TetrMesh* mesh)
{

//cout << "GML1: " << endl;
	if(create_random_axis(node, mesh) < 0)
	{
		if(logger != NULL)
			logger->write(string("ERROR: GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced::get_max_lambda - failed axis randomization!"));
		return -1;
	}

//cout << "GML2: " << endl;
	int node_num = node->local_num;

	// We return max(l) * sqrt((la+2*mu)/rho)
	float l2;
	float max_l2 = 0;
	for(int i = 0; i < 3; i++) {
		l2 = random_axis_inv[node_num].ksi[0][i] * random_axis_inv[node_num].ksi[0][i]
				+ random_axis_inv[node_num].ksi[1][i] * random_axis_inv[node_num].ksi[1][i]
				+ random_axis_inv[node_num].ksi[2][i] * random_axis_inv[node_num].ksi[2][i];
		if(l2 > max_l2)
			max_l2 = l2;
	}

//cout << "GML3:  " << endl;
cout << "L2: " << max_l2 << "\n";
	return sqrt( max_l2 * ( (node->la) + 2*(node->mu) ) / (node->rho) );
};

int GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced::create_random_axis(ElasticNode* cur_node, TetrMesh* mesh)
{

//cout << "CRA1: " << endl;
	if(basis_quantity < (mesh->nodes).size()) {
		basis_quantity = (mesh->nodes).size();
		random_axis = (basis*)realloc(random_axis, sizeof(basis) * basis_quantity);
		random_axis_inv = (basis*)realloc(random_axis_inv, sizeof(basis) * basis_quantity);
	}

	int node_num = cur_node->local_num;
	int neigh = (cur_node->elements)->size();

	if(neigh > 2) {

//cout << "CRA2.1: " << endl;
		int num[3];
		// Get 3 random different numbers of neighbour tetrs
		num[0] = rand() % neigh;
		num[1] = rand() % neigh;
		while(num[1] == num[0])
			num[1] = (num[1] + 1) % neigh;
		num[2] = rand() % neigh;
		while((num[2] == num[1]) || (num[2] == num[0]))
			num[2] = (num[2] + 1) % neigh;

		// Create basis on these tetrs
		create_basis_based_on_tetrs(cur_node, mesh, num[0], num[1], num[2]);
		normalize_basis(node_num);

		// Check its volume
		int volume_ok = check_basis_volume(node_num);

		#ifdef DEBUG_AXIS_RAND
		if(logger != NULL) {
			stringstream ss;
			ss.setf(ios::fixed,ios::floatfield);
			ss.precision(10);
			ss << "DEBUG: GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced::create random axis" << endl;
			ss << "NODE " << node_num << ": x: " << cur_node->coords[0]
					 << " y: " << cur_node->coords[1] << " z: " << cur_node->coords[2];
			ss << endl << "NEIGH: " << neigh << endl;
			ss << "Based on tetr. Pass 0." << endl;
			ss << "NUM1: " << num[0] << " NUM2: " << num[1] << " NUM3: " << num[2] << endl;
			for(int i = 0; i < 3; i++)
				ss << "KSI[" << i << "]: x: " << random_axis[node_num].ksi[i][0]
						<< " y: " << random_axis[node_num].ksi[i][1]
						<< " z: " << random_axis[node_num].ksi[i][2] << endl;
			ss << "VOLUME_OK: " << volume_ok << endl;
			logger->write(ss.str());
		}
		#endif

		// Number of additional tetr to replace num3 if necessary
		int replacement_tetr = 0;

		// If volume is bad - loop through possible replacements
		while( (volume_ok < 0) && (replacement_tetr < neigh) ) {
			// ... and try to create new basis with num3 replaced with additional tetr
			if( (replacement_tetr != num[0]) && (replacement_tetr != num[1]) && (replacement_tetr != num[2]) ) {
				create_basis_based_on_tetrs(cur_node, mesh, num[0], num[1], replacement_tetr);
				normalize_basis(node_num);
				volume_ok = check_basis_volume(node_num);

				#ifdef DEBUG_AXIS_RAND
				if(logger != NULL) {
					stringstream ss;
					ss.setf(ios::fixed,ios::floatfield);
					ss.precision(10);
					ss << "DEBUG: GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced::create random axis";
					ss << endl << "NODE " << node_num <<": x: " << cur_node->coords[0] 
							<< " y: " << cur_node->coords[1] << " z: " << cur_node->coords[2];
					ss << endl << "NEIGH: " << neigh << endl;
					ss << "Based on tetr. Fixing with repl " << replacement_tetr << endl;
					ss << "NUM1: "<< num[0] << " NUM2: "<< num[1] << " NUM3: "<< replacement_tetr <<endl;
					for(int i = 0; i < 3; i++)
						ss << "KSI[" << i << "]: x: " << random_axis[node_num].ksi[i][0]
								<< " y: " << random_axis[node_num].ksi[i][1]
								<< " z: " << random_axis[node_num].ksi[i][2] << endl;
					ss << "VOLUME_OK: " << volume_ok << endl;
					logger->write(ss.str());
				}
				#endif
			}
			replacement_tetr++;
		}

		// If we are out of additional tetrs and basis is still bad - try to build it on 1 neigh tetr chosed at random
		if( volume_ok < 0) {
			int neigh_num = rand() % neigh;
			Tetrahedron* tmp_tetr = mesh->get_tetrahedron( (cur_node->elements)->at(neigh_num) );
			create_basis_based_on_verts(cur_node, mesh, tmp_tetr);
			normalize_basis(node_num);

			#ifdef DEBUG_AXIS_RAND
			if(logger != NULL) {
				stringstream ss;
				ss.setf(ios::fixed,ios::floatfield);
				ss.precision(10);
				ss << "DEBUG: GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced::create random axis" << endl;
				ss << "NODE " << node_num << ": x: " << cur_node->coords[0] 
							<< " y: " << cur_node->coords[1] << " z: " << cur_node->coords[2];
				ss << endl << "NEIGH: " << neigh << endl;
				ss << "Based on tetr. Last chance." << endl;
				ss << "NUM: " << neigh_num << endl;
				for(int i = 0; i < 3; i++)
					ss << "KSI[" << i << "]: x: " << random_axis[node_num].ksi[i][0]
							<< " y: " << random_axis[node_num].ksi[i][1]
							<< " z: " << random_axis[node_num].ksi[i][2] << endl;
				ss << "VOLUME_OK: " << volume_ok << endl;
				logger->write(ss.str());
			}
			#endif
		}
		// TODO - we can loop through neigh tetrs if random one was bad

	} else {
//cout << "CRA2.2: " << endl;
//cout << "\t\t" << neigh << endl;
		// Special case, only 1 or 2 tetrs available
		// If there are 2 tetrs - choose one at random. If there is one - take it.
		int neigh_num = rand() % neigh;
		Tetrahedron* tmp_tetr = mesh->get_tetrahedron( (cur_node->elements)->at(neigh_num) );

//cout << "CRA2.2.1: " << endl;
		// ... and try to build basis on this 1 neigh tetr
		create_basis_based_on_verts(cur_node, mesh, tmp_tetr);
		normalize_basis(node_num);

		#ifdef DEBUG_AXIS_RAND
		if(logger != NULL) {
			stringstream ss;
			ss.setf(ios::fixed,ios::floatfield);
			ss.precision(10);
			ss << "DEBUG: GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced::create random axis" << endl;
			ss << "NODE " << node_num << ": x: " << cur_node->coords[0] 
						<< " y: " << cur_node->coords[1] << " z: " << cur_node->coords[2];
			ss << endl << "NEIGH: " << neigh << endl;
			ss << "Based on nodes." << endl;
			ss << "NUM: " << neigh_num << endl;
			for(int i = 0; i < 3; i++)
				ss << "KSI[" << i << "]: x: " << random_axis[node_num].ksi[i][0]
						<< " y: " << random_axis[node_num].ksi[i][1]
						<< " z: " << random_axis[node_num].ksi[i][2] << endl;
			logger->write(ss.str());
		}
		#endif
		// TODO - we can loop through neigh tetrs if random one was bad
	}

//cout << "CRA3: " << endl;
	// Final check for basis volume
	if(check_basis_volume(node_num) < 0) {
		if(logger != NULL) {
			stringstream ss;
			ss.setf(ios::fixed,ios::floatfield);
			ss.precision(10);
			ss << "ERROR: GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced::create random axis - bad basis - vectors are too close!" << endl;
			ss << "NODE " << node_num << ": x: " << cur_node->coords[0] 
						<< " y: " << cur_node->coords[1] << " z: " << cur_node->coords[2];
			ss << endl << "NEIGH: " << neigh << endl;
			for(int i = 0; i < 3; i++)
				ss << "KSI[" << i << "]: x: " << random_axis[node_num].ksi[i][0]
						<< " y: " << random_axis[node_num].ksi[i][1]
						<< " z: " << random_axis[node_num].ksi[i][2] << endl;
			//ss << "VOLUME: " << check_basis_volume(node_num) << endl;
			logger->write(ss.str());
		}
		return -1;
	}

//cout << "CRA4: " << endl;
	// If basis is left - make it right
	fix_left_right_basis(node_num);

//cout << "CRA5: " << endl;
	// Create inverse matrix
	find_inverse_matrix(node_num);

//cout << "CRA6: " << endl;
	return 0;
};


void GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced::create_basis_based_on_tetrs(ElasticNode* cur_node, TetrMesh* mesh, int n1, int n2, int n3)
{
	int num[3];
	num[0] = n1; num[1] = n2; num[2] = n3;

	Tetrahedron* tmp_tetr;
	ElasticNode* tmp_node;
	int neigh_tetr_index;

	int node_num = cur_node->local_num;

	// Create basis - axis go to the centers of random neighbour tetrs
	for(int i = 0; i < 3; i++)
	{
		neigh_tetr_index = (cur_node->elements)->at(num[i]);
		tmp_tetr = mesh->get_tetrahedron(neigh_tetr_index);
		// Create vector
		for(int j = 0; j < 3; j++) {
			random_axis[node_num].ksi[i][j] = 0;
			// Center of tetr (average coordinate of verticles) minus coordinate of current node
			for(int k = 0; k < 4; k++) {
				tmp_node = mesh->get_node(tmp_tetr->vert[k]);
				random_axis[node_num].ksi[i][j] += tmp_node->coords[j];
			}
			random_axis[node_num].ksi[i][j] = (random_axis[node_num].ksi[i][j] / 4) - cur_node->coords[j];
		}
	}
};


void GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced::create_basis_based_on_verts(ElasticNode* cur_node, TetrMesh* mesh, Tetrahedron* tetr)
{
	int count = 0;
	ElasticNode* target_nodes[4];
	ElasticNode center_node;

	int node_num = cur_node->local_num;

	// Take verts of chosen tetr (except cur_node one)
	for(int i = 0; i < 4; i++)
		if(tetr->vert[i] != node_num) {
			target_nodes[count] = mesh->get_node(tetr->vert[i]);
			count++;
		}

	// Find center of facet formed by these verts
	for(int i = 0; i < 3; i++)
		center_node.coords[i] = ( (target_nodes[0])->coords[i] + (target_nodes[1])->coords[i]
						+ (target_nodes[2])->coords[i] ) / 3;

	// Create basis - axis go to the verts (with small correction to the center of the facet to fit into tetr)
	for(int i = 0; i < 3; i++)
		for(int j = 0; j < 3; j++)
			random_axis[node_num].ksi[i][j] = (target_nodes[i])->coords[j] 
			   - 0.1 * ( (target_nodes[i])->coords[j] - center_node.coords[j] ) - cur_node->coords[j];
};


int GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced::check_basis_volume(int node_num)
{
	// Check if basis is good enough:
	float volume = qm_engine.tetr_volume(
			random_axis[node_num].ksi[0][0], random_axis[node_num].ksi[0][1], random_axis[node_num].ksi[0][2], 
			random_axis[node_num].ksi[1][0], random_axis[node_num].ksi[1][1], random_axis[node_num].ksi[1][2], 
			random_axis[node_num].ksi[2][0], random_axis[node_num].ksi[2][1], random_axis[node_num].ksi[2][2]);

	// ... if vectors are too close - we can not work with this basis
	if(fabs(volume) < MIN_ALLOWED_VOLUME)
		return -1;

	return 0;
};


// TODO - may be should just add tmp norm into check_volume
void GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced::normalize_basis(int node_num)
{
	// Normalize vectors
	float l;
	for (int i = 0; i < 3; i++) {
		l = sqrt( random_axis[node_num].ksi[i][0] * random_axis[node_num].ksi[i][0] 
				+ random_axis[node_num].ksi[i][1] * random_axis[node_num].ksi[i][1]
				+ random_axis[node_num].ksi[i][2] * random_axis[node_num].ksi[i][2] );
		random_axis[node_num].ksi[i][0] /= l;
		random_axis[node_num].ksi[i][1] /= l;
		random_axis[node_num].ksi[i][2] /= l;
	}
};


void GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced::fix_left_right_basis(int node_num)
{
	float det = qm_engine.determinant(random_axis[node_num].ksi[0][0], random_axis[node_num].ksi[0][1], 
			random_axis[node_num].ksi[0][2], random_axis[node_num].ksi[1][0], 
			random_axis[node_num].ksi[1][1], random_axis[node_num].ksi[1][2], 
			random_axis[node_num].ksi[2][0], random_axis[node_num].ksi[2][1], random_axis[node_num].ksi[2][2]);

	float ftmp;

	// ... if basis is left one - swap 2nd and 3rd vectors to get right one
	if(det < 0) {
		for(int i = 0; i < 3; i++) {
			ftmp = random_axis[node_num].ksi[1][i];
			random_axis[node_num].ksi[1][i] = random_axis[node_num].ksi[2][i];
			random_axis[node_num].ksi[2][i] = ftmp;
		}
	}
};


void GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced::find_inverse_matrix(int node_num)
{
	// Find inverse matrix
	float det = qm_engine.determinant(
			random_axis[node_num].ksi[0][0], random_axis[node_num].ksi[0][1], random_axis[node_num].ksi[0][2],
			random_axis[node_num].ksi[1][0], random_axis[node_num].ksi[1][1], random_axis[node_num].ksi[1][2],
			random_axis[node_num].ksi[2][0], random_axis[node_num].ksi[2][1], random_axis[node_num].ksi[2][2] );

	random_axis_inv[node_num].ksi[0][0] = ( random_axis[node_num].ksi[1][1] * random_axis[node_num].ksi[2][2] 
						- random_axis[node_num].ksi[2][1] * random_axis[node_num].ksi[1][2]) / det;
	random_axis_inv[node_num].ksi[1][0] =  ( random_axis[node_num].ksi[2][0] * random_axis[node_num].ksi[1][2]
						- random_axis[node_num].ksi[1][0] * random_axis[node_num].ksi[2][2]) / det;
	random_axis_inv[node_num].ksi[2][0] = ( random_axis[node_num].ksi[1][0] * random_axis[node_num].ksi[2][1]
						- random_axis[node_num].ksi[2][0] * random_axis[node_num].ksi[1][1]) / det;

	random_axis_inv[node_num].ksi[0][1] = ( random_axis[node_num].ksi[2][1] * random_axis[node_num].ksi[0][2]
						- random_axis[node_num].ksi[0][1] * random_axis[node_num].ksi[2][2]) / det;
	random_axis_inv[node_num].ksi[1][1] =  ( random_axis[node_num].ksi[0][0] * random_axis[node_num].ksi[2][2]
						- random_axis[node_num].ksi[2][0] * random_axis[node_num].ksi[0][2]) / det;
	random_axis_inv[node_num].ksi[2][1] =  ( random_axis[node_num].ksi[2][0] * random_axis[node_num].ksi[0][1]
						- random_axis[node_num].ksi[0][0] * random_axis[node_num].ksi[2][1]) / det;

	random_axis_inv[node_num].ksi[0][2] =  ( random_axis[node_num].ksi[0][1] * random_axis[node_num].ksi[1][2]
						- random_axis[node_num].ksi[1][1] * random_axis[node_num].ksi[0][2]) / det;
	random_axis_inv[node_num].ksi[1][2] =  ( random_axis[node_num].ksi[1][0] * random_axis[node_num].ksi[0][2]
						- random_axis[node_num].ksi[0][0] * random_axis[node_num].ksi[1][2]) / det;
	random_axis_inv[node_num].ksi[2][2] =  ( random_axis[node_num].ksi[0][0] * random_axis[node_num].ksi[1][1]
						- random_axis[node_num].ksi[1][0] * random_axis[node_num].ksi[0][1]) / det;

};


// TODO - may be should just add it to find_inverse
/*void GCM_Tetr_Plastic_Interpolation_1stOrder_Advanced::conormalize_matrixes(int node_num)
{
	// Normalize vectors
	float l;
	for (int i = 0; i < 3; i++) {
		l = sqrt( random_axis[node_num].ksi[i][0] * random_axis[node_num].ksi[i][0] 
				+ random_axis[node_num].ksi[i][1] * random_axis[node_num].ksi[i][1]
				+ random_axis[node_num].ksi[i][2] * random_axis[node_num].ksi[i][2] );
		random_axis[node_num].ksi[i][0] /= l;
		random_axis[node_num].ksi[i][1] /= l;
		random_axis[node_num].ksi[i][2] /= l;
	}
};*/

