GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis::GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis()
{
	num_method_type.assign("1st order interpolation on tetr mesh (with plasticity) - axis rotation");
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

GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis::~GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis()
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

int GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis::prepare_part_step(ElasticNode* cur_node, int stage)
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

void GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis::drop_deviator(ElasticNode* cur_node, ElasticNode* new_node)
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

int GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis::do_next_part_step(ElasticNode* cur_node, ElasticNode* new_node, float time_step, int stage, TetrMesh* mesh)
{

	if(stage == 3) {
		drop_deviator(cur_node, new_node);
		return 0;
	}

	int node_num = cur_node->local_num;

	if (cur_node->border_type == BORDER)
		mesh->find_border_node_normal(node_num, &outer_normal[0], &outer_normal[1], &outer_normal[2]);

	//  Prepare matrixes  A, Lambda, Omega, Omega^(-1)
	if (prepare_part_step(cur_node, stage) < 0)
		return -1;

	// Here we will store (omega = Matrix_OMEGA * u)
	float omega[9];

	for(int i = 0; i < 9; i++)
		dksi[i] = - elastic_matrix3d[stage]->L(i,i) * time_step;

	float alpha = 0;

	// Number of outer characteristics
	int outer_count;

	do {
		outer_count = find_nodes_on_previous_time_layer(cur_node, stage, mesh, alpha);
		alpha += 0.1;
	} while( (outer_count != 0) && (outer_count != 3) && (alpha < 1.01) );

	// TODO - merge this condition with the next ones
	if((outer_count != 0) && (outer_count != 3)) {
		if(logger != NULL)
		{
			stringstream ss;
			ss.setf(ios::fixed,ios::floatfield);
			ss.precision(10);
			ss << "ERROR: GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis::do_next_part_step - there are " << outer_count << " 'outer' characteristics." << endl;
			ss << "STAGE: " << stage << endl;
			ss << "NODE " << node_num << ": x: " << cur_node->coords[0] 
						<< " y: " << cur_node->coords[1]
						<< " z: " << cur_node->coords[2] << endl;
			if( cur_node->border_type == BORDER )
				ss << "BORDER" << endl;
			else
				ss << "INNER" << endl;
			ss << "OUTER_NORMAL: " << outer_normal[0] << " " << outer_normal[1] << " " << outer_normal[2] << endl;
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

	// If all the omegas are 'inner'
	// omega = Matrix_OMEGA * u
	// new_u = Matrix_OMEGA^(-1) * omega
	// TODO - to think - if all omegas are 'inner' can we skip matrix calculations and just use new_u = interpolated_u ?
	if( outer_count == 0 )
	{
		// Calculate omega value
		for(int i = 0; i < 9; i++)
		{
			// omega on new time layer is equal to omega on previous time layer along characteristic
			omega[i] = 0;
			for(int j = 0; j < 9; j++)
			{
				omega[i] += elastic_matrix3d[stage]->U(i,j) 
							* previous_nodes[ppoint_num[i]].values[j];
			}
		}
		// Calculate new values
		for(int i = 0; i < 9; i++)
		{
			new_node->values[i] = 0;
			for(int j = 0; j < 9; j++)
			{
				new_node->values[i] += elastic_matrix3d[stage]->U1(i,j) * omega[j];
			}
		}
	}
	// If there are 3 'outer' omegas - we should use border or contact algorithm
	// TODO - ... we should also use it when l*t/h > 1 and there is 1 'outer' omega
	// 		(we should add 2 more corresponding omegas to 'outer' manually
	else if ( outer_count == 3 )
	{
		// Check contact state
		// If both directions show no contact - use border algorithm, otherwise use contact algorithm
		// TODO - handle non-border nodes with contact_data == NULL separately

		// Border algorithm
		if( (cur_node->contact_data != NULL) && ( cur_node->contact_data->axis_plus[stage] == -1 ) && ( cur_node->contact_data->axis_minus[stage] == -1 ) )
		{

			// Tmp value for GSL solver
			int s;

			// Fixed border algorithm
			/*for(int i = 0; i < 9; i++)
			{
				// If omega is 'inner' one
				if(inner[i])
				{
					// Calculate omega value
					omega[i] = 0;
					for(int j = 0; j < 9; j++)
					{
						omega[i] += elastic_matrix3d[stage]->U(i,j) 
									* previous_nodes[ppoint_num[i]].values[j];
					}
					// Load appropriate values into GSL containers
					gsl_vector_set(om_gsl, i, omega[i]);
					for(int j = 0; j < 9; j++)
						gsl_matrix_set(U_gsl, i, j, elastic_matrix3d[stage]->U(i,j));
				}
				// If omega is 'outer' one
				else
				{
					// omega (as right-hand part of OLE) is zero - it is not-moving border
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
							logger->write(string("ERROR: GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis::do_next_part_step - wrong stage number!"));
						return -1;
					}
				}
			}*/

			// Free border algorithm
			for(int i = 0; i < 9; i++)
			{
				// If omega is 'inner' one
				if(inner[i])
				{
					// Calculate omega value
					omega[i] = 0;
					for(int j = 0; j < 9; j++)
					{
						omega[i] += elastic_matrix3d[stage]->U(i,j) 
									* previous_nodes[ppoint_num[i]].values[j];
					}
					// Load appropriate values into GSL containers
					gsl_vector_set(om_gsl, i, omega[i]);
					for(int j = 0; j < 9; j++)
						gsl_matrix_set(U_gsl, i, j, elastic_matrix3d[stage]->U(i,j));
				}
				// If omega is 'outer' one
				else
				{
					// omega (as right-hand part of OLE) is zero - it is free border, no external stress
					gsl_vector_set(om_gsl, i, 0);
					// corresponding string in matrix is zero ...
					for(int j = 0; j < 9; j++)
						gsl_matrix_set(U_gsl, i, j, 0);

					// ... except normal and tangential stress
					// We use outer normal to find total stress vector (sigma * n) - sum of normal and shear - and tell it is zero
					// TODO - never-ending questions - is everything ok with (x-y-z) and (ksi-eta-dzeta) basises?
					if( stage < 3 )
					{
						if ( outer_count == 3 ) {
							gsl_matrix_set(U_gsl, i, 3, outer_normal[0]);
							gsl_matrix_set(U_gsl, i, 4, outer_normal[1]);
							gsl_matrix_set(U_gsl, i, 5, outer_normal[2]);
							outer_count--;
						} else if ( outer_count == 2 ) {
							gsl_matrix_set(U_gsl, i, 4, outer_normal[0]);
							gsl_matrix_set(U_gsl, i, 6, outer_normal[1]);
							gsl_matrix_set(U_gsl, i, 7, outer_normal[2]);
							outer_count--;
						} else if ( outer_count == 1 ) {
							gsl_matrix_set(U_gsl, i, 5, outer_normal[0]);
							gsl_matrix_set(U_gsl, i, 7, outer_normal[1]);
							gsl_matrix_set(U_gsl, i, 8, outer_normal[2]);
							outer_count--;
						}
					}
					else 
					{
						if(logger != NULL)
							logger->write(string("ERROR: GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis::do_next_part_step - wrong stage number!"));
						return -1;
					}
				}
			}

			// Solve linear equations using GSL tools
			gsl_linalg_LU_decomp (U_gsl, p_gsl, &s);
			gsl_linalg_LU_solve (U_gsl, p_gsl, om_gsl, x_gsl);

			for(int j = 0; j < 9; j++)
				new_node->values[j] = gsl_vector_get(x_gsl, j);

		// Contact algorithm
		// TODO - copy idea from do_next_part_contact from @sedire and make it working with random axis
		} else {
			cout << "Contact not implemented!\n";
			exit(-1);
		}
	}
	// If there are 'outer' omegas but not 3 ones - we should not be here - we checked it before
	else
	{
		return -1;
	}

	return 0;
};

int GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis::find_nodes_on_previous_time_layer(ElasticNode* cur_node, int stage, TetrMesh* mesh, float alpha)
{

	if( (alpha > 1.01) || (alpha < 0) ) {
		if(logger != NULL)
			logger->write(string("ERROR: GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis::find_nodes_on_previous_time_layer - bad alpha!"));
		return -1;
	}

	if (stage >= 3) {
		if(logger != NULL)
			logger->write(string("ERROR: GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis::find_nodes_on_previous_time_layer - bad stage number!"));
		return -1;
	}

	int node_num = cur_node->local_num;

	// Just tmp tetr pointer
	Tetrahedron* tmp_tetr;

	int count = 0;

	float dx[3];
	float dx_ksi[3];
	float dx_ksi_normal_projection[3];

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

			// ... Find vectors ...
			for(int j = 0; j < 3; j++)
				dx_ksi[j] = dksi[i] * random_axis[node_num].ksi[stage][j];

			float dx_ksi_normal_projection_modul = dx_ksi[0] * outer_normal[0] + dx_ksi[1] * outer_normal[1] + dx_ksi[2] * outer_normal[2];

			for(int j = 0; j < 3; j++)
				dx_ksi_normal_projection[j] = dx_ksi_normal_projection_modul * outer_normal[j];

			// ... Calculate coordinates ...
			for(int j = 0; j < 3; j++) {
				dx[j] = dx_ksi[j] * (1 - alpha) + dx_ksi_normal_projection[j] * alpha;
				previous_nodes[count].coords[j] = cur_node->coords[j] + dx[j];
			}

			// ... Find owner tetrahedron ...
			tmp_tetr = mesh->find_owner_tetr(cur_node, dx[0], dx[1], dx[2]);

			if( tmp_tetr != NULL )
			{
				// ... And interpolate values
				if(mesh->interpolate(&previous_nodes[count], tmp_tetr) < 0)
				{
					if(logger != NULL)
						logger->write(string("ERROR: GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis::find_nodes_on_previous_time_layer - interpolation failed!"));
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
	for(int i = 0; i < 9; i++)
		if(!inner[i])
			outer_count++;

	return outer_count;
};

int GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis::get_number_of_stages()
{
	return 4;
};

float GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis::get_max_lambda(ElasticNode* node, TetrMesh* mesh)
{

	if(create_random_axis(node, mesh) < 0)
	{
		if(logger != NULL)
			logger->write(string("ERROR: GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis::get_max_lambda - failed axis randomization!"));
		return -1;
	}

	// We just return sqrt((la+2*mu)/rho) because axis are randomized by rotation, so x^2+y^2+z^2 == 1
	// TODO - explicit check?
	return sqrt( ( (node->la) + 2 * (node->mu) ) / (node->rho) );
};

int GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis::create_random_axis(ElasticNode* cur_node, TetrMesh* mesh)
{

	if(basis_quantity < (mesh->nodes).size()) {
		basis_quantity = (mesh->nodes).size();
		random_axis = (basis*)realloc(random_axis, sizeof(basis) * basis_quantity);
		random_axis_inv = (basis*)realloc(random_axis_inv, sizeof(basis) * basis_quantity);
	}

	int node_num = cur_node->local_num;
	const double PI = atan(1) * 4;

	if(cur_node->border_type == INNER) {

		// TODO think about limits - PI or 2*PI
		float phi = (rand() % 360) * 2 * PI / 360;
		float psi = (rand() % 360) * 2 * PI / 360;
		float teta = (rand() % 360) * 2 * PI / 360;

		create_rotation_matrix(node_num, phi, psi, teta);

	} else if (cur_node->border_type == BORDER) {

		mesh->find_border_node_normal(node_num, &outer_normal[0], &outer_normal[1], &outer_normal[2]);

		// TODO think about limits - PI or 2*PI
		float teta = (rand() % 360) * 2 * PI / 360;

		// Rotate at random angle around normal
		create_rotation_matrix(node_num, outer_normal[0], outer_normal[1], outer_normal[2], teta);

	} else {
		if(logger != NULL)
			logger->write(string("ERROR: GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis::create_random_axis - unknown border type!"));
		return -1;
	}

	// Attach new basis to node - we need it for CollisionDetector to create virtual nodes using directions of these axis
	// TODO - should it be this way at all?
	cur_node->local_basis = &random_axis[node_num];

	find_inverse_matrix(node_num);

	return 0;
};

void GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis::create_rotation_matrix(int node_num, float phi, float psi, float teta)
{
	random_axis[node_num].ksi[0][0] = cos(teta) * cos(psi);
	random_axis[node_num].ksi[0][1] = cos(teta) * sin(psi);
	random_axis[node_num].ksi[0][2] = - sin(teta);

	random_axis[node_num].ksi[1][0] = - cos(phi) * sin(psi) + sin(phi) * sin(teta) * cos(psi);
	random_axis[node_num].ksi[1][1] = cos(phi) * cos(psi) + sin(phi) * sin(teta) * sin(psi);
	random_axis[node_num].ksi[1][2] = sin(phi) * cos(teta);

	random_axis[node_num].ksi[2][0] = sin(phi) * sin(psi) + cos(phi) * sin(teta) * cos(psi);
	random_axis[node_num].ksi[2][1] = - sin(phi) * cos(psi) + cos(phi) * sin(teta) * sin(psi);
	random_axis[node_num].ksi[2][2] = cos(phi) * cos(teta);
};

void GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis::create_rotation_matrix(int node_num, float x, float y, float z, float teta)
{
	float sqrt3 = sqrt(3);
	float sqrt2 = sqrt(2);
	float sqrt6 = sqrt2 * sqrt3;

	basis new_axis;

	// Rotation from normal "quasi-(1;1;1)" to first axis "quasi-(1;0;0)"
	float x_m[3][3];
	x_m[0][0] = sqrt3/3;	x_m[1][0] = sqrt3/3;	x_m[2][0] = sqrt3/3;
	x_m[0][1] = -sqrt2/2;	x_m[1][1] = sqrt2/2;	x_m[2][1] = 0;
	x_m[0][2] = -sqrt6/6;	x_m[1][2] = -sqrt6/6;	x_m[2][2] = 2*sqrt6/6;

	for(int i = 0; i < 3; i++)
		new_axis.ksi[0][i] = x_m[0][i] * x + x_m[1][i] * y + x_m[2][i] * z;

	// Rotation from normal "quasi-(1;1;1)" to second axis "quasi-(0;1;0)"
	float y_m[3][3];
	y_m[0][0] = sqrt2/2;	y_m[1][0] = -sqrt2/2;	y_m[2][0] = 0;
	y_m[0][1] = sqrt3/3;	y_m[1][1] = sqrt3/3;	y_m[2][1] = sqrt3/3;
	y_m[0][2] = -sqrt6/6;	y_m[1][2] = -sqrt6/6;	y_m[2][2] = 2*sqrt6/6;

	for(int i = 0; i < 3; i++)
		new_axis.ksi[1][i] = y_m[0][i] * x + y_m[1][i] * y + y_m[2][i] * z;

	// Rotation from normal "quasi-(1;1;1)" to third axis "quasi-(0;0;1)"
	float z_m[3][3];
	z_m[0][0] = sqrt2/2;	z_m[1][0] = 0;		z_m[2][0] = -sqrt2/2;
	z_m[0][1] = -sqrt6/6;	z_m[1][1] = 2*sqrt6/6;	z_m[2][1] = -sqrt6/6;
	z_m[0][2] = sqrt3/3;	z_m[1][2] = sqrt3/3;	z_m[2][2] = sqrt3/3;

	for(int i = 0; i < 3; i++)
		new_axis.ksi[2][i] = z_m[0][i] * x + z_m[1][i] * y + z_m[2][i] * z;

	// Rotation matrix - random teta around normal
	float rot[3][3];

	rot[0][0] = cos(teta) + x * x * (1 - cos(teta));
	rot[0][1] = y * x * (1 - cos(teta)) + z * sin(teta);
	rot[0][2] = z * x * (1 - cos(teta)) - y * sin(teta);

	rot[1][0] = y * x * (1 - cos(teta)) - z * sin(teta);
	rot[1][1] = cos(teta) + y * y * (1 - cos(teta));
	rot[1][2] = y * z * (1 - cos(teta)) + x * sin(teta);

	rot[2][0] = x * z * (1 - cos(teta)) + y * sin(teta);
	rot[2][1] = y * z * (1 - cos(teta)) - x * sin(teta);
	rot[2][2] = cos(teta) + z * z * (1 - cos(teta));

	// Rotate
	for(int i = 0; i < 3; i++) {
		for(int j = 0; j < 3; j++) {
			random_axis[node_num].ksi[i][j] = 0;
			for(int k = 0; k < 3; k++) {
				random_axis[node_num].ksi[i][j] += rot[k][j] * new_axis.ksi[i][k];
			}
		}
	}

};

void GCM_Tetr_Plastic_Interpolation_1stOrder_Rotate_Axis::find_inverse_matrix(int node_num)
{
	// Find inverse matrix, use the fact that it is rotation matrix

	random_axis_inv[node_num].ksi[0][0] = random_axis[node_num].ksi[0][0];
	random_axis_inv[node_num].ksi[1][0] = random_axis[node_num].ksi[0][1];
	random_axis_inv[node_num].ksi[2][0] = random_axis[node_num].ksi[0][2];

	random_axis_inv[node_num].ksi[0][1] = random_axis[node_num].ksi[1][0];
	random_axis_inv[node_num].ksi[1][1] = random_axis[node_num].ksi[1][1];
	random_axis_inv[node_num].ksi[2][1] = random_axis[node_num].ksi[1][2];

	random_axis_inv[node_num].ksi[0][2] = random_axis[node_num].ksi[2][0];
	random_axis_inv[node_num].ksi[1][2] = random_axis[node_num].ksi[2][1];
	random_axis_inv[node_num].ksi[2][2] = random_axis[node_num].ksi[2][2];

};

