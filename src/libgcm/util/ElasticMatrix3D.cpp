#include "libgcm/util/ElasticMatrix3D.hpp"
#include "libgcm/materials/IsotropicElasticMaterial.hpp"
#include "libgcm/node/CalcNode.hpp"

void gcm::ElasticMatrix3D::initializeAx(const Material* material, gcm_matrix& A, gcm_matrix& L, gcm_matrix& U, gcm_matrix& U1)
{
    A.clear();
    L.clear();
    U.clear();
    U1.clear();

#ifdef NDEBUG
    const auto mat = static_cast<const IsotropicElasticMaterial*>(material);
#else
    const auto mat = dynamic_cast<const IsotropicElasticMaterial*>(material);
    assert_true(mat);
#endif

    auto rho = mat->getRho();
    auto mu = mat->getMu();
    auto la = mat->getLambda();

    A(0, 3) = -1 / rho;
    A(1, 4) = -1 / rho;
    A(2, 5) = -1 / rho;
    A(3, 0) = -la - 2 * mu;
    A(4, 1) = -mu;
    A(5, 2) = -mu;
    A(6, 0) = -la;
    A(8, 0) = -la;

    L(0, 0) = sqrt(mu / rho);
    L(1, 1) = sqrt(mu / rho);
    L(2, 2) = -sqrt(mu / rho);
    L(3, 3) = -sqrt(mu / rho);
    L(7, 7) = sqrt((la + 2 * mu) / rho);
    L(8, 8) = -sqrt((la + 2 * mu) / rho);

    U(0, 1) = -sqrt(rho * mu)*0.5;
    U(0, 4) = 0.5;
    U(1, 2) = -sqrt(rho * mu)*0.5;
    U(1, 5) = 0.5;
    U(2, 1) = sqrt(rho * mu)*0.5;
    U(2, 4) = 0.5;
    U(3, 2) = sqrt(rho * mu)*0.5;
    U(3, 5) = 0.5;
    U(4, 7) = 1;
    U(5, 3) = -la / (la + 2 * mu);
    U(5, 6) = 1;
    U(6, 3) = -la / (la + 2 * mu);
    U(6, 8) = 1;
    U(7, 0) = -0.5 * la * sqrt(rho / (la + 2 * mu));
    U(7, 3) = 0.5 * la / (la + 2 * mu);
    U(8, 0) = 0.5 * la * sqrt(rho / (la + 2 * mu));
    U(8, 3) = 0.5 * la / (la + 2 * mu);

    U1(0, 7) = -sqrt((la + 2 * mu) / rho) / la;
    U1(0, 8) = sqrt((la + 2 * mu) / rho) / la;
    U1(1, 0) = -1 / sqrt(rho * mu);
    U1(1, 2) = 1 / sqrt(rho * mu);
    U1(2, 1) = -1 / sqrt(rho * mu);
    U1(2, 3) = 1 / sqrt(rho * mu);
    U1(3, 7) = (la + 2 * mu) / la;
    U1(3, 8) = (la + 2 * mu) / la;
    U1(4, 0) = 1;
    U1(4, 2) = 1;
    U1(5, 1) = 1;
    U1(5, 3) = 1;
    U1(6, 5) = 1;
    U1(6, 7) = 1;
    U1(6, 8) = 1;
    U1(7, 4) = 1;
    U1(8, 6) = 1;
    U1(8, 7) = 1;
    U1(8, 8) = 1;
};

void gcm::ElasticMatrix3D::initializeAy(const Material* material, gcm_matrix& A, gcm_matrix& L, gcm_matrix& U, gcm_matrix& U1)
{
    A.clear();
    L.clear();
    U.clear();
    U1.clear();

#ifdef NDEBUG
    const auto mat = reinterpret_cast<const IsotropicElasticMaterial*>(material);
#else
    const auto mat = dynamic_cast<const IsotropicElasticMaterial*>(material);
    assert_true(mat);
#endif

    auto rho = mat->getRho();
    auto mu = mat->getMu();
    auto la = mat->getLambda();

    A(0, 4) = -1 / rho;
    A(1, 6) = -1 / rho;
    A(2, 7) = -1 / rho;
    A(3, 1) = -la;
    A(4, 0) = -mu;
    A(6, 1) = -la - 2 * mu;
    A(7, 2) = -mu;
    A(8, 1) = -la;

    L(0, 0) = sqrt((la + 2 * mu) / rho);
    L(1, 1) = -sqrt((la + 2 * mu) / rho);
    L(2, 2) = sqrt(mu / rho);
    L(3, 3) = sqrt(mu / rho);
    L(4, 4) = -sqrt(mu / rho);
    L(5, 5) = -sqrt(mu / rho);

    U(0, 1) = -0.5 * la * sqrt(rho / (la + 2 * mu));
    U(0, 6) = 0.5 * la / (la + 2 * mu);
    U(1, 1) = 0.5 * la * sqrt(rho / (la + 2 * mu));
    U(1, 6) = 0.5 * la / (la + 2 * mu);
    U(2, 0) = -sqrt(rho * mu)*0.5;
    U(2, 4) = 0.5;
    U(3, 2) = -sqrt(rho * mu)*0.5;
    U(3, 7) = 0.5;
    U(4, 0) = sqrt(rho * mu)*0.5;
    U(4, 4) = 0.5;
    U(5, 2) = sqrt(rho * mu)*0.5;
    U(5, 7) = 0.5;
    U(6, 3) = 1;
    U(6, 6) = -la / (la + 2 * mu);
    U(7, 5) = 1;
    U(8, 6) = -la / (la + 2 * mu);
    U(8, 8) = 1;

    U1(0, 2) = -1 / sqrt(rho * mu);
    U1(0, 4) = 1 / sqrt(rho * mu);
    U1(1, 0) = -sqrt((la + 2 * mu) / rho) / la;
    U1(1, 1) = sqrt((la + 2 * mu) / rho) / la;
    U1(2, 3) = -1 / sqrt(rho * mu);
    U1(2, 5) = 1 / sqrt(rho * mu);
    U1(3, 0) = 1;
    U1(3, 1) = 1;
    U1(3, 6) = 1;
    U1(4, 2) = 1;
    U1(4, 4) = 1;
    U1(5, 7) = 1;
    U1(6, 0) = (la + 2 * mu) / la;
    U1(6, 1) = (la + 2 * mu) / la;
    U1(7, 3) = 1;
    U1(7, 5) = 1;
    U1(8, 0) = 1;
    U1(8, 1) = 1;
    U1(8, 8) = 1;
};

void gcm::ElasticMatrix3D::initializeAz(const Material* material, gcm_matrix& A, gcm_matrix& L, gcm_matrix& U, gcm_matrix& U1)
{
    A.clear();
    L.clear();
    U.clear();
    U1.clear();

#ifdef NDEBUG
    const auto mat = reinterpret_cast<const IsotropicElasticMaterial*>(material);
#else
    const auto mat = dynamic_cast<const IsotropicElasticMaterial*>(material);
    assert_true(mat);
#endif

    auto rho = mat->getRho();
    auto mu = mat->getMu();
    auto la = mat->getLambda();

    A(0, 5) = -1 / rho;
    A(1, 7) = -1 / rho;
    A(2, 8) = -1 / rho;
    A(3, 2) = -la;
    A(5, 0) = -mu;
    A(6, 2) = -la;
    A(7, 1) = -mu;
    A(8, 2) = -la - 2 * mu;

    L(3, 3) = sqrt((la + 2 * mu) / rho);
    L(4, 4) = -sqrt((la + 2 * mu) / rho);
    L(5, 5) = sqrt(mu / rho);
    L(6, 6) = sqrt(mu / rho);
    L(7, 7) = -sqrt(mu / rho);
    L(8, 8) = -sqrt(mu / rho);

    U(0, 6) = 1;
    U(0, 8) = -la / (la + 2 * mu);
    U(1, 3) = 1;
    U(1, 8) = -la / (la + 2 * mu);
    U(2, 4) = 1;
    U(3, 2) = -0.5 * la * sqrt(rho / (la + 2 * mu));
    U(3, 8) = 0.5 * la / (la + 2 * mu);
    U(4, 2) = 0.5 * la * sqrt(rho / (la + 2 * mu));
    U(4, 8) = 0.5 * la / (la + 2 * mu);
    U(5, 0) = 0.5;
    U(5, 5) = -0.5 / sqrt(rho * mu);
    U(6, 1) = 0.5;
    U(6, 7) = -0.5 / sqrt(rho * mu);
    U(7, 0) = 0.5;
    U(7, 5) = 0.5 / sqrt(rho * mu);
    U(8, 1) = 0.5;
    U(8, 7) = 0.5 / sqrt(rho * mu);

    U1(0, 5) = 1;
    U1(0, 7) = 1;
    U1(1, 6) = 1;
    U1(1, 8) = 1;
    U1(2, 3) = -sqrt((la + 2 * mu) / rho) / la;
    U1(2, 4) = sqrt((la + 2 * mu) / rho) / la;
    U1(3, 1) = 1;
    U1(3, 3) = 1;
    U1(3, 4) = 1;
    U1(4, 2) = 1;
    U1(5, 5) = -sqrt(rho * mu);
    U1(5, 7) = sqrt(rho * mu);
    U1(6, 0) = 1;
    U1(6, 3) = 1;
    U1(6, 4) = 1;
    U1(7, 6) = -sqrt(rho * mu);
    U1(7, 8) = sqrt(rho * mu);
    U1(8, 3) = (la + 2 * mu) / la;
    U1(8, 4) = (la + 2 * mu) / la;
}
