#include "libgcm/rheology/decomposers/IsotropicRheologyMatrixDecomposer.hpp"

#include <cmath>

using namespace gcm;

void IsotropicRheologyMatrixDecomposer::decomposeX(const gcm_matrix& a, gcm_matrix& u, gcm_matrix& l, gcm_matrix& u1) const
{
    l.clear();
    u.clear();
    u1.clear();

    auto rho = -1 / a.get(0, 3);
    auto mu = -a.get(4, 1);
    auto la = -a.get(6, 0);
    
    l(0, 0) = sqrt(mu / rho);
    l(1, 1) = sqrt(mu / rho);
    l(2, 2) = -sqrt(mu / rho);
    l(3, 3) = -sqrt(mu / rho);
    l(7, 7) = sqrt((la + 2 * mu) / rho);
    l(8, 8) = -sqrt((la + 2 * mu) / rho);

    u(0, 1) = -sqrt(rho * mu)*0.5;
    u(0, 4) = 0.5;
    u(1, 2) = -sqrt(rho * mu)*0.5;
    u(1, 5) = 0.5;
    u(2, 1) = sqrt(rho * mu)*0.5;
    u(2, 4) = 0.5;
    u(3, 2) = sqrt(rho * mu)*0.5;
    u(3, 5) = 0.5;
    u(4, 7) = 1;
    u(5, 3) = -la / (la + 2 * mu);
    u(5, 6) = 1;
    u(6, 3) = -la / (la + 2 * mu);
    u(6, 8) = 1;
    u(7, 0) = -0.5 * la * sqrt(rho / (la + 2 * mu));
    u(7, 3) = 0.5 * la / (la + 2 * mu);
    u(8, 0) = 0.5 * la * sqrt(rho / (la + 2 * mu));
    u(8, 3) = 0.5 * la / (la + 2 * mu);

    u1(0, 7) = -sqrt((la + 2 * mu) / rho) / la;
    u1(0, 8) = sqrt((la + 2 * mu) / rho) / la;
    u1(1, 0) = -1 / sqrt(rho * mu);
    u1(1, 2) = 1 / sqrt(rho * mu);
    u1(2, 1) = -1 / sqrt(rho * mu);
    u1(2, 3) = 1 / sqrt(rho * mu);
    u1(3, 7) = (la + 2 * mu) / la;
    u1(3, 8) = (la + 2 * mu) / la;
    u1(4, 0) = 1;
    u1(4, 2) = 1;
    u1(5, 1) = 1;
    u1(5, 3) = 1;
    u1(6, 5) = 1;
    u1(6, 7) = 1;
    u1(6, 8) = 1;
    u1(7, 4) = 1;
    u1(8, 6) = 1;
    u1(8, 7) = 1;
    u1(8, 8) = 1;
}

void IsotropicRheologyMatrixDecomposer::decomposeY(const gcm_matrix& a, gcm_matrix& u, gcm_matrix& l, gcm_matrix& u1) const
{
    l.clear();
    u.clear();
    u1.clear();

    auto rho = -1 / a.get(0, 4);
    auto mu = -a.get(4, 0);
    auto la = -a.get(3, 1);
    
    l(0, 0) = sqrt((la + 2 * mu) / rho);
    l(1, 1) = -sqrt((la + 2 * mu) / rho);
    l(2, 2) = sqrt(mu / rho);
    l(3, 3) = sqrt(mu / rho);
    l(4, 4) = -sqrt(mu / rho);
    l(5, 5) = -sqrt(mu / rho);

    u(0, 1) = -0.5 * la * sqrt(rho / (la + 2 * mu));
    u(0, 6) = 0.5 * la / (la + 2 * mu);
    u(1, 1) = 0.5 * la * sqrt(rho / (la + 2 * mu));
    u(1, 6) = 0.5 * la / (la + 2 * mu);
    u(2, 0) = -sqrt(rho * mu)*0.5;
    u(2, 4) = 0.5;
    u(3, 2) = -sqrt(rho * mu)*0.5;
    u(3, 7) = 0.5;
    u(4, 0) = sqrt(rho * mu)*0.5;
    u(4, 4) = 0.5;
    u(5, 2) = sqrt(rho * mu)*0.5;
    u(5, 7) = 0.5;
    u(6, 3) = 1;
    u(6, 6) = -la / (la + 2 * mu);
    u(7, 5) = 1;
    u(8, 6) = -la / (la + 2 * mu);
    u(8, 8) = 1;

    u1(0, 2) = -1 / sqrt(rho * mu);
    u1(0, 4) = 1 / sqrt(rho * mu);
    u1(1, 0) = -sqrt((la + 2 * mu) / rho) / la;
    u1(1, 1) = sqrt((la + 2 * mu) / rho) / la;
    u1(2, 3) = -1 / sqrt(rho * mu);
    u1(2, 5) = 1 / sqrt(rho * mu);
    u1(3, 0) = 1;
    u1(3, 1) = 1;
    u1(3, 6) = 1;
    u1(4, 2) = 1;
    u1(4, 4) = 1;
    u1(5, 7) = 1;
    u1(6, 0) = (la + 2 * mu) / la;
    u1(6, 1) = (la + 2 * mu) / la;
    u1(7, 3) = 1;
    u1(7, 5) = 1;
    u1(8, 0) = 1;
    u1(8, 1) = 1;
    u1(8, 8) = 1;
}

void IsotropicRheologyMatrixDecomposer::decomposeZ(const gcm_matrix& a, gcm_matrix& u, gcm_matrix& l, gcm_matrix& u1) const
{
    l.clear();
    u.clear();
    u1.clear();

    auto rho = -1 / a.get(0, 5);
    auto mu = -a.get(5, 0);
    auto la = -a.get(3, 2);
    
    l(3, 3) = sqrt((la + 2 * mu) / rho);
    l(4, 4) = -sqrt((la + 2 * mu) / rho);
    l(5, 5) = sqrt(mu / rho);
    l(6, 6) = sqrt(mu / rho);
    l(7, 7) = -sqrt(mu / rho);
    l(8, 8) = -sqrt(mu / rho);

    u(0, 6) = 1;
    u(0, 8) = -la / (la + 2 * mu);
    u(1, 3) = 1;
    u(1, 8) = -la / (la + 2 * mu);
    u(2, 4) = 1;
    u(3, 2) = -0.5 * la * sqrt(rho / (la + 2 * mu));
    u(3, 8) = 0.5 * la / (la + 2 * mu);
    u(4, 2) = 0.5 * la * sqrt(rho / (la + 2 * mu));
    u(4, 8) = 0.5 * la / (la + 2 * mu);
    u(5, 0) = 0.5;
    u(5, 5) = -0.5 / sqrt(rho * mu);
    u(6, 1) = 0.5;
    u(6, 7) = -0.5 / sqrt(rho * mu);
    u(7, 0) = 0.5;
    u(7, 5) = 0.5 / sqrt(rho * mu);
    u(8, 1) = 0.5;
    u(8, 7) = 0.5 / sqrt(rho * mu);

    u1(0, 5) = 1;
    u1(0, 7) = 1;
    u1(1, 6) = 1;
    u1(1, 8) = 1;
    u1(2, 3) = -sqrt((la + 2 * mu) / rho) / la;
    u1(2, 4) = sqrt((la + 2 * mu) / rho) / la;
    u1(3, 1) = 1;
    u1(3, 3) = 1;
    u1(3, 4) = 1;
    u1(4, 2) = 1;
    u1(5, 5) = -sqrt(rho * mu);
    u1(5, 7) = sqrt(rho * mu);
    u1(6, 0) = 1;
    u1(6, 3) = 1;
    u1(6, 4) = 1;
    u1(7, 6) = -sqrt(rho * mu);
    u1(7, 8) = sqrt(rho * mu);
    u1(8, 3) = (la + 2 * mu) / la;
    u1(8, 4) = (la + 2 * mu) / la;
}

