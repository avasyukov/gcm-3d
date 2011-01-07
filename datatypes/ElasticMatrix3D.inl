ElasticMatrix3D::ElasticMatrix3D()
{
	A.resize(9,9);
	L.resize(9,9);
	U.resize(9,9);
	U1.resize(9,9);
	zero_all();
};

ElasticMatrix3D::~ElasticMatrix3D() { };

int ElasticMatrix3D::prepare_matrix(float la, float mu, float ro, int stage)
{
	return prepare_matrix(la, mu, ro, stage, NULL);
};

int ElasticMatrix3D::prepare_matrix(float la, float mu, float ro, int stage, Logger* logger)
{
	if((la <= 0) || (mu <= 0) || (ro <= 0))
	{
		if(logger != NULL) {
			logger->write(string("ERROR: ElasticMatrix3D::prepare_matrix - bad rheology"));
			// TODO add details and diagnostics - stage, la, mu, ro
		}
		return -1;
	}

	if (stage == 0) {
		CreateAx(la, mu, ro);
	} else if (stage == 1) { 
		CreateAy(la, mu, ro); 
	} else if (stage == 2) { 
		CreateAz(la, mu, ro); 
	} else {
		if(logger != NULL) {
			logger->write(string("ERROR: ElasticMatrix3D::prepare_matrix - wrong stage number"));
			// TODO add details and diagnostics
		}
		return -1;
	}
	return 0;
};

int ElasticMatrix3D::self_check(float la, float mu, float ro, Logger* logger)
{
	for (int i = 0; i < 3; i++)
	{
		if (prepare_matrix(la, mu, ro, i, logger) < 0) return -1;
		if (check_current(logger) < 0) return -1;
	}
	if(logger != NULL)
		logger->write(string("INFO: ElasticMatrix3D::self_check - OK"));
	return 0;
};

int ElasticMatrix3D::check_current(Logger* logger)
{
	ublas_matrix Z(9,9);
	Z = (prod(U1, ublas_matrix(prod(L,U)) ) - A);
	float max_l = max_lambda();
	for (unsigned i = 0; i < 9; ++i)
		for (unsigned j = 0; j < 9; ++j)
			if(fabs(Z(i,j)) > 0.0001 * max_l) {
				if(logger != NULL) {
					logger->write(string("ERROR: ElasticMatrix3D::check_current failed"));
					// TODO add details and diagnostics
				}	
				return -1;
			}
	return 0;
}

float ElasticMatrix3D::max_lambda()
{
	float res = 0;
	for (unsigned i = 0; i < 9; ++i)
		if(L(i,i) > res)
			res = L(i,i);
	return res;
};

void ElasticMatrix3D::zero_all()
{
	for (unsigned i = 0; i < 9; ++i)
		for (unsigned j = 0; j < 9; ++j)
		{
			A(i,j) = 0;
			L(i,j) = 0;
			U(i,j) = 0;
			U1(i,j) = 0;
		}
};

void ElasticMatrix3D::CreateAx(float la, float mu, float ro)
{
	zero_all();

	A(0,3) = -1/ro;
	A(1,4) = -1/ro;
	A(2,5) = -1/ro;
	A(3,0) = -la - 2*mu;
	A(4,1) = -mu;
	A(5,2) = -mu;
	A(6,0) = -la;
	A(8,0) = -la;

	L(0,0) = sqrt(mu/ro);
	L(1,1) = sqrt(mu/ro);
	L(2,2) = -sqrt(mu/ro);
	L(3,3) = -sqrt(mu/ro);
	L(7,7) = sqrt((la+2*mu)/ro);
	L(8,8) = -sqrt((la+2*mu)/ro);

	U(0,1) = -sqrt(ro*mu)*0.5;
	U(0,4) = 0.5;
	U(1,2) = -sqrt(ro*mu)*0.5;
	U(1,5) = 0.5;
	U(2,1) = sqrt(ro*mu)*0.5;
	U(2,4) = 0.5;
	U(3,2) = sqrt(ro*mu)*0.5;
	U(3,5) = 0.5;
	U(4,7) = 1;
	U(5,3) = -la/(la+2*mu);
	U(5,6) = 1;
	U(6,3) = -la/(la+2*mu);
	U(6,8) = 1;
	U(7,0) = -0.5*la * sqrt(ro/(la+2*mu));
	U(7,3) = 0.5*la/(la+2*mu);
	U(8,0) = 0.5*la * sqrt(ro/(la+2*mu));
	U(8,3) = 0.5*la/(la+2*mu);

	U1(0,7) = -sqrt((la+2*mu)/ro)/la;
	U1(0,8) = sqrt((la+2*mu)/ro)/la;
	U1(1,0) = -1/sqrt(ro*mu);
	U1(1,2) = 1/sqrt(ro*mu);
	U1(2,1) = -1/sqrt(ro*mu);
	U1(2,3) = 1/sqrt(ro*mu);
	U1(3,7) = (la+2*mu)/la;
	U1(3,8) = (la+2*mu)/la;
	U1(4,0) = 1;
	U1(4,2) = 1;
	U1(5,1) = 1;
	U1(5,3) = 1;
	U1(6,5) = 1;
	U1(6,7) = 1;
	U1(6,8) = 1;
	U1(7,4) = 1;
	U1(8,6) = 1;
	U1(8,7) = 1;
	U1(8,8) = 1;
};

void ElasticMatrix3D::CreateAy(float la, float mu, float ro)
{
	zero_all();

	A(0,4) = -1/ro;
	A(1,6) = -1/ro;
	A(2,7) = -1/ro;
	A(3,1) = -la;
	A(4,0) = -mu;
	A(6,1) = -la-2*mu;
	A(7,2) = -mu;
	A(8,1) = -la;

	L(0,0) = sqrt((la+2*mu)/ro);
	L(1,1) = -sqrt((la+2*mu)/ro);
	L(2,2) = sqrt(mu/ro);
	L(3,3) = sqrt(mu/ro);
	L(4,4) = -sqrt(mu/ro);
	L(5,5) = -sqrt(mu/ro);

	U(0,1) = -0.5*la*sqrt(ro/(la+2*mu));
	U(0,6) = 0.5*la/(la+2*mu);
	U(1,1) = 0.5*la*sqrt(ro/(la+2*mu));
	U(1,6) = 0.5*la/(la+2*mu);
	U(2,0) = -sqrt(ro*mu)*0.5;
	U(2,4) = 0.5;
	U(3,2) = -sqrt(ro*mu)*0.5;
	U(3,7) = 0.5;
	U(4,0) = sqrt(ro*mu)*0.5;
	U(4,4) = 0.5;
	U(5,2) = sqrt(ro*mu)*0.5;
	U(5,7) = 0.5;
	U(6,3) = 1;
	U(6,6) = -la/(la+2*mu);
	U(7,5) = 1;
	U(8,6) = -la/(la+2*mu);
	U(8,8) = 1;

	U1(0,2) = -1/sqrt(ro*mu);
	U1(0,4) = 1/sqrt(ro*mu);
	U1(1,0) = -sqrt((la+2*mu)/ro)/la;
	U1(1,1) = sqrt((la+2*mu)/ro)/la;
	U1(2,3) = -1/sqrt(ro*mu);
	U1(2,5) = 1/sqrt(ro*mu);
	U1(3,0) = 1;
	U1(3,1) = 1;
	U1(3,6) = 1;
	U1(4,2) = 1;
	U1(4,4) = 1;
	U1(5,7) = 1;
	U1(6,0) = (la+2*mu)/la;
	U1(6,1) = (la+2*mu)/la;
	U1(7,3) = 1;
	U1(7,5) = 1;
	U1(8,0) = 1;
	U1(8,1) = 1;
	U1(8,8) = 1;
};

void ElasticMatrix3D::CreateAz(float la, float mu, float ro)
{
	zero_all();

	A(0,5) = -1/ro;
	A(1,7) = -1/ro;
	A(2,8) = -1/ro;
	A(3,2) = -la;
	A(5,0) = -mu;
	A(6,2) = -la;
	A(7,1) = -mu;
	A(8,2) = -la-2*mu;

	L(3,3) = sqrt((la+2*mu)/ro);
	L(4,4) =-sqrt((la+2*mu)/ro);
	L(5,5) = sqrt(mu/ro);
	L(6,6) = sqrt(mu/ro);
	L(7,7) = - sqrt(mu/ro);
	L(8,8) = - sqrt(mu/ro);

	U(0,6) = 1;
	U(0,8) = -la/(la+2*mu);
	U(1,3) = 1;
	U(1,8) = -la/(la+2*mu);
	U(2,4) = 1;
	U(3,2) = -0.5*la*sqrt(ro/(la+2*mu));
	U(3,8) = 0.5*la/(la+2*mu);
	U(4,2) = 0.5*la*sqrt(ro/(la+2*mu));
	U(4,8) = 0.5*la/(la+2*mu);
	U(5,0) = 0.5;
	U(5,5) = -0.5/sqrt(ro*mu);
	U(6,1) = 0.5;
	U(6,7) = -0.5/sqrt(ro*mu);
	U(7,0) = 0.5;
	U(7,5) = 0.5/sqrt(ro*mu);
	U(8,1) = 0.5;
	U(8,7) = 0.5/sqrt(ro*mu);

	U1(0,5) = 1;
	U1(0,7) = 1;
	U1(1,6) = 1;
	U1(1,8) = 1;
	U1(2,3) = -sqrt((la+2*mu)/ro)/la;
	U1(2,4) = sqrt((la+2*mu)/ro)/la;
	U1(3,1) = 1;
	U1(3,3) = 1;
	U1(3,4) = 1;
	U1(4,2) = 1;
	U1(5,5) = -sqrt(ro*mu);
	U1(5,7) = sqrt(ro*mu);
	U1(6,0) = 1;
	U1(6,3) = 1;
	U1(6,4) = 1;
	U1(7,6) = -sqrt(ro*mu);
	U1(7,8) = sqrt(ro*mu);
	U1(8,3) = (la+2*mu)/la;
	U1(8,4) = (la+2*mu)/la;
};
