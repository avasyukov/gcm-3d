/*
 * File:   StaticCalc.cpp
 * Author: anganar
 */
#include "libgcm/method/StaticCalc.hpp"
#include "libgcm/mesh/tetr/TetrMeshSecondOrder.hpp"
#include "libgcm/node/CalcNode.hpp"
#include "libgcm/calc/border/BorderCalculator.hpp"
#include "libgcm/calc/border/ExternalForceCalculator.hpp"
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <vector>
#include <string>

using std::string;
using namespace gcm;

/* Static calc impl */

struct Element
{
    void CalculateStiffnessMatrix(const Eigen::Matrix<float, 6, 6>& D, std::vector<Eigen::Triplet<float> >& triplets,
                                  const Eigen::VectorXf& nodesX, const Eigen::VectorXf& nodesY, const Eigen::VectorXf& nodesZ);

    Eigen::Matrix<float, 6, 12> B;
    int nodesIds[4];
};

void Element::CalculateStiffnessMatrix(const Eigen::Matrix<float, 6, 6>& D, std::vector<Eigen::Triplet<float> >& triplets,
                                       const Eigen::VectorXf& nodesX, const Eigen::VectorXf& nodesY, const Eigen::VectorXf& nodesZ)
{
    Eigen::Vector4f x, y, z;
    x << nodesX[nodesIds[0]], nodesX[nodesIds[1]], nodesX[nodesIds[2]], nodesX[nodesIds[3]];
    y << nodesY[nodesIds[0]], nodesY[nodesIds[1]], nodesY[nodesIds[2]], nodesY[nodesIds[3]];
    z << nodesZ[nodesIds[0]], nodesZ[nodesIds[1]], nodesZ[nodesIds[2]], nodesZ[nodesIds[3]];

    Eigen::Matrix4f C;
    C << Eigen::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), x, y, z;

    Eigen::Matrix4f IC = C.inverse();

    for (int i = 0; i < 4; i++)
    {
        float Nix = IC(1, i);
        float Niy = IC(2, i);
        float Niz = IC(3, i);
        B(0, 3 * i + 0) = Nix;
        B(0, 3 * i + 1) = 0.0f;
        B(0, 3 * i + 2) = 0.0f;

        B(1, 3 * i + 0) = 0.0f;
        B(1, 3 * i + 1) = Niy;
        B(1, 3 * i + 2) = 0.0f;

        B(2, 3 * i + 0) = 0.0f;
        B(2, 3 * i + 1) = 0.0f;
        B(2, 3 * i + 2) = Niz;

        B(3, 3 * i + 0) = Niy;
        B(3, 3 * i + 1) = Nix;
        B(3, 3 * i + 2) = 0.0f;

        B(4, 3 * i + 0) = Niz;
        B(4, 3 * i + 1) = 0.0f;
        B(4, 3 * i + 2) = Nix;

        B(5, 3 * i + 0) = 0.0f;
        B(5, 3 * i + 1) = Niz;
        B(5, 3 * i + 2) = Niy;
    }
    Eigen::Matrix<float, 12, 12> K = B.transpose() * D * B * std::abs(C.determinant()) / 6.0f;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            Eigen::Triplet<float> trplt11(3 * nodesIds[i] + 0, 3 * nodesIds[j] + 0, K(3 * i + 0, 3 * j + 0));
            Eigen::Triplet<float> trplt12(3 * nodesIds[i] + 0, 3 * nodesIds[j] + 1, K(3 * i + 0, 3 * j + 1));
            Eigen::Triplet<float> trplt13(3 * nodesIds[i] + 0, 3 * nodesIds[j] + 2, K(3 * i + 0, 3 * j + 2));

            Eigen::Triplet<float> trplt21(3 * nodesIds[i] + 1, 3 * nodesIds[j] + 0, K(3 * i + 1, 3 * j + 0));
            Eigen::Triplet<float> trplt22(3 * nodesIds[i] + 1, 3 * nodesIds[j] + 1, K(3 * i + 1, 3 * j + 1));
            Eigen::Triplet<float> trplt23(3 * nodesIds[i] + 1, 3 * nodesIds[j] + 2, K(3 * i + 1, 3 * j + 2));

            Eigen::Triplet<float> trplt31(3 * nodesIds[i] + 2, 3 * nodesIds[j] + 0, K(3 * i + 2, 3 * j + 0));
            Eigen::Triplet<float> trplt32(3 * nodesIds[i] + 2, 3 * nodesIds[j] + 1, K(3 * i + 2, 3 * j + 1));
            Eigen::Triplet<float> trplt33(3 * nodesIds[i] + 2, 3 * nodesIds[j] + 2, K(3 * i + 2, 3 * j + 2));

            triplets.push_back(trplt11);
            triplets.push_back(trplt12);
            triplets.push_back(trplt13);
            triplets.push_back(trplt21);
            triplets.push_back(trplt22);
            triplets.push_back(trplt23);
            triplets.push_back(trplt31);
            triplets.push_back(trplt32);
            triplets.push_back(trplt33);
        }
    }
}

void StaticCalc::SetConstraints(Eigen::SparseMatrix<float>::InnerIterator& it, int index)
{
    if (it.row() == index || it.col() == index)
    {
        it.valueRef() = it.row() == it.col() ? 1.0f : 0.0f;
    }
}

void StaticCalc::ApplyConstraints(Eigen::SparseMatrix<float>& K, const std::vector<Constraint>& constraints)
{
    std::vector<int> indicesToConstraint;

    for (std::vector<Constraint>::const_iterator it = constraints.begin(); it != constraints.end(); ++it)
    {
        if (it->type & Constraint::UX)
        {
            indicesToConstraint.push_back(3 * it->node + 0);
        }
        if (it->type & Constraint::UY)
        {
            indicesToConstraint.push_back(3 * it->node + 1);
        }
        if (it->type & Constraint::UZ)
        {
            indicesToConstraint.push_back(3 * it->node + 2);
        }
    }

    for (int k = 0; k < K.outerSize(); ++k)
    {
        for (Eigen::SparseMatrix<float>::InnerIterator it(K, k); it; ++it)
        {
            for (std::vector<int>::iterator idit = indicesToConstraint.begin(); idit != indicesToConstraint.end(); ++idit)
            {
                SetConstraints(it, *idit);
            }
        }
    }
}

/* Calc impl end, wrappers start */

StaticCalc::StaticCalc() {
    INIT_LOGGER("gcm.method.StaticCalc");
}

StaticCalc::~StaticCalc()
{
}

int StaticCalc::getNumberOfStages()
{
    return 1;
}

void StaticCalc::doNextPartStep(CalcNode& cur_node, CalcNode& new_node, float time_step, int stage, Mesh* mesh)
{
    TetrMeshSecondOrder* tmesh = (TetrMeshSecondOrder*)mesh;
    int number_of_nodes = tmesh->firstOrderNodesNumber;
    int number_of_tetrs = tmesh->getTetrsNumber();

    LOG_INFO("Found " << number_of_nodes << " nodes and " << number_of_tetrs << " tetrs");

    int                      	nodesCount = number_of_nodes;
    Eigen::VectorXf          	nodesX;
    Eigen::VectorXf          	nodesY;
    Eigen::VectorXf          	nodesZ;
    Eigen::VectorXf          	loads;
    std::vector< Element >   	elements;
    std::vector< Constraint >	constraints;

    float la = mesh->getNode(0).getMaterial()->getLa();
    float mu = mesh->getNode(0).getMaterial()->getMu();
    LOG_INFO("Material props: la " << la << " mu " << mu);
    //float poissonRatio, youngModulus;
    //float la = youngModulus * poissonRatio / ((1 + poissonRatio) * (1 - 2 * poissonRatio));
    //float mu = youngModulus / (2 * (1 + poissonRatio));

    Eigen::Matrix<float, 6, 6> D;
    D <<
            la + 2 * mu,    la, 	        la,             0.0f,       0.0f,       0.0f,
            la,             la + 2 * mu, 	la,             0.0f,       0.0f,       0.0f,
            la,             la, 	        la + 2 * mu,    0.0f,       0.0f,       0.0f,
            0.0f,           0.0f,           0.0f,           mu,         0.0f,       0.0f,
            0.0f,           0.0f,           0.0f,           0.0f,       mu,         0.0f,
            0.0f,           0.0f,           0.0f,           0.0f,       0.0f,       mu
            ;

    nodesX.resize(nodesCount);
    nodesY.resize(nodesCount);
    nodesZ.resize(nodesCount);
    for(int i = 0; i < nodesCount; i++) {
        CalcNode& node = mesh->getNode(i);
        nodesX[i] = node.coords[0];
        nodesY[i] = node.coords[1];
        nodesZ[i] = node.coords[2];
    }

    int elementCount = number_of_tetrs;

    for (MapIter itr = tmesh->tetrsMap.begin(); itr != tmesh->tetrsMap.end(); ++itr) {
        int i = itr->first;
        TetrFirstOrder& t = tmesh->getTetr2(i);
        Element element;
        for(int j = 0; j < 4; j++)
            element.nodesIds[j] = t.verts[j];
        elements.push_back(element);
    }

    loads.resize(3 * nodesCount);
    loads.setZero();

    for(int i = 0; i < nodesCount; i++) {
        CalcNode &node = mesh->getNode(i);
        if(node.getBorderConditionId() > 1) {
            BorderCalculator* calc = Engine::getInstance().getBorderCondition(node.getBorderConditionId())->calc;
            string condType = calc->getType();
            if(condType == "FixedBorderCalculator")  {
                //std::cout << "Constraint on node " << node.number << " " << node.coords << std::endl;
                Constraint constraint;
                constraint.node = node.number;
                constraint.type = static_cast<Constraint::Type>(7);
                constraints.push_back(constraint);
            } else if(condType == "ExternalForceCalculator") {
                float* load_dir = ((ExternalForceCalculator*)calc)->tangential_direction;
                float load[3];
                load[0] = load_dir[0] * ((ExternalForceCalculator*)calc)->tangential_stress;
                load[1] = load_dir[1] * ((ExternalForceCalculator*)calc)->tangential_stress;
                load[2] = load_dir[2] * ((ExternalForceCalculator*)calc)->tangential_stress;
                //std::cout << "Load on node " << node.number << " " << node.coords
                //          << ": " << load[0] << " " << load[1] << " " << load[2] << std::endl;
                loads[3 * node.number + 0] = load[0];
                loads[3 * node.number + 1] = load[1];
                loads[3 * node.number + 2] = load[2];
            }
        }
    }


    std::vector<Eigen::Triplet<float> > triplets;
    for (std::vector<Element>::iterator it = elements.begin(); it != elements.end(); ++it)
    {
        it->CalculateStiffnessMatrix(D, triplets, nodesX, nodesY, nodesZ);
    }

    Eigen::SparseMatrix<float> globalK(3 * nodesCount, 3 * nodesCount);
    globalK.setFromTriplets(triplets.begin(), triplets.end());

    ApplyConstraints(globalK, constraints);

    Eigen::SparseLU<Eigen::SparseMatrix<float> > solver(globalK);

    LOG_INFO("Starting main solve cycle");
    Eigen::VectorXf displacements = solver.solve(loads);
    LOG_INFO("Completed main solve cycle");

    for (std::vector<Element>::iterator it = elements.begin(); it != elements.end(); ++it)
    {
        Eigen::Matrix<float, 12, 1> delta;
        delta << displacements.segment<3>(3 * it->nodesIds[0]),
                displacements.segment<3>(3 * it->nodesIds[1]),
                displacements.segment<3>(3 * it->nodesIds[2]),
                displacements.segment<3>(3 * it->nodesIds[3]);

        Eigen::Matrix<float, 6, 1> sigma = D * it->B * delta;
        double sigma_mises = sqrt(0.5 * (
                (sigma[0] - sigma[1]) * (sigma[0] - sigma[1])
                + (sigma[0] - sigma[2]) * (sigma[0] - sigma[2])
                + (sigma[1] - sigma[2]) * (sigma[1] - sigma[2])
                + 6.0f * (sigma[3] * sigma[3] + sigma[4] * sigma[4] + sigma[5] * sigma[5])
        ));
        //std::cout << displacements << std::endl;
        //std::cout << sigma_mises << std::endl;

        for(int i = 0; i < 4; i++) {
            int num = it->nodesIds[i];
            Eigen::Vector3f d = displacements.segment<3>(3 * num);

            CalcNode& node = mesh->getNode(num);
            node.coords[0] += d[0];
            node.coords[1] += d[1];
            node.coords[2] += d[2];
            node.sxx = sigma[0];
            node.syy = sigma[1];
            node.szz = sigma[2];
            node.sxy = sigma[3];
            node.sxz = sigma[4];
            node.syz = sigma[5];
        }
    }

    /*for(int i = 0; i < nodesCount; i++) {
        Eigen::Vector3f d = displacements.segment<3>(3 * i);
        CalcNode& node = mesh->getNode(i);
        node.coords[0] += d[0];
        node.coords[1] += d[1];
        node.coords[2] += d[2];
    }*/
}

string StaticCalc::getType()
{
    return "StaticCalc";
}
