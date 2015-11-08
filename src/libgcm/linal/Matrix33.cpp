#include "libgcm/linal/Matrix33.hpp"
#include "libgcm/util/Assertion.hpp"

using namespace gcm;
using namespace gcm::linal;

real gcm::linal::determinant(const Matrix33& m)
{
    return m.a11*(m.a22*m.a33-m.a23*m.a32) - m.a12*(m.a21*m.a33-m.a23*m.a31) + m.a13*(m.a21*m.a32-m.a22*m.a31);
};

Matrix33 gcm::linal::rotate(const Matrix33& m, real a1, real a2, real a3)
{
	auto G1 = [](real a) {
		return Matrix33({
			1.0,		0.0,		0.0,
			0.0,		cos(a),		-sin(a),
			0.0,		sin(a),		cos(a)
		});
	};

	auto G2 = [](real a) {
		return Matrix33({
			cos(a),		0.0,		sin(a),
			0.0,		1.0,		0.0,
			-sin(a),	0.0,		cos(a)
		});
	};

	auto G3 = [](real a) {
		return Matrix33({
			cos(a),		-sin(a),	0.0,
			sin(a),		cos(a),		0.0,
			0.0,		0.0,		1.0
		});
	};

	return G3(-a3) * G2(-a2) * G1(-a1) * m * G1(a1) * G2(a2) * G3(a3);
};
