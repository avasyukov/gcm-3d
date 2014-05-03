/*
 * File:   InterpolationFixedAxis.cpp
 * Author: anganar
 *
 * Created on May 3, 2013, 12:00 AM
 */
#include "libgcm/method/InterpolationFixedAxis.hpp"

string gcm::InterpolationFixedAxis::getType()
{
    return "InterpolationFixedAxis";
}

gcm::InterpolationFixedAxis::InterpolationFixedAxis()
{
    INIT_LOGGER("gcm.method.InterpolationFixedAxis");
}

gcm::InterpolationFixedAxis::~InterpolationFixedAxis()
{
}

int gcm::InterpolationFixedAxis::getNumberOfStages()
{
    return 3;
}

void gcm::InterpolationFixedAxis::doNextPartStep(CalcNode& cur_node, CalcNode& new_node, float time_step, int stage, Mesh* mesh)
{
    assert_ge(stage, 0);
    assert_le(stage, 2);

    IEngine* engine = mesh->getBody()->getEngine();

    LOG_TRACE("Start node prepare for node " << cur_node.number);
    LOG_TRACE("Node: " << cur_node);

    // Variables used in calculations internally

    // Delta x on previous time layer for all the omegas
    //     omega_new_time_layer(ksi) = omega_old_time_layer(ksi+dksi)
    float dksi[9];

    // If the corresponding point on previous time layer is inner or not
    bool inner[9];

    // We will store interpolated nodes on previous time layer here
    // We know that we need five nodes for each direction (corresponding to Lambdas -C1, -C2, 0, C2, C1)
    // TODO  - We can  deal with (lambda == 0) separately
    vector<CalcNode> previous_nodes;
    previous_nodes.resize(9);

    // Outer normal at current point
    float outer_normal[3];

    // Number of outer characteristics
    int outer_count = prepare_node(cur_node, cur_node.getRheologyMatrix(),
                                   time_step, stage, mesh,
                                   dksi, inner, previous_nodes,
                                   outer_normal);

    LOG_TRACE("Done node prepare");

    // If all the omegas are 'inner'
    // omega = Matrix_OMEGA * u
    // new_u = Matrix_OMEGA^(-1) * omega
    // TODO - to think - if all omegas are 'inner' can we skip matrix calculations and just use new_u = interpolated_u ?
    if (cur_node.isInner()) {
        LOG_TRACE("Start inner node calc");
        if (outer_count == 0)
            // FIXME - hardcoded name
            engine->getVolumeCalculator("SimpleVolumeCalculator")->doCalc(
                                                                          new_node, cur_node.getRheologyMatrix(), previous_nodes);
        else
            THROW_BAD_MESH("Outer characteristic for internal node detected");
        LOG_TRACE("Done inner node calc");
    }

    if (cur_node.isBorder()) {
        LOG_TRACE("Start border node calc");
        // FIXME - do smth with this!
        // Think about: (a) cube, (b) rotated cube, (c) sphere.
        outer_normal[0] = outer_normal[1] = outer_normal[2] = 0;
        outer_normal[stage] = 1;
        // If there is no 'outer' omega - it is ok, border node can be inner for some directions
        if (outer_count == 0) {
            // FIXME - hardcoded name
            engine->getVolumeCalculator("SimpleVolumeCalculator")->doCalc(
                                                                          new_node, cur_node.getRheologyMatrix(), previous_nodes);
        }
            // If there are 3 'outer' omegas - we should use border or contact algorithm
        else if (outer_count == 3) {
            // Border
            if (!cur_node.isInContact() || cur_node.contactDirection != stage) {
                // FIXME
                int borderCondId = cur_node.getBorderConditionId();
                LOG_TRACE("Using calculator: " << engine->getBorderCondition(borderCondId)->calc->getType());
                engine->getBorderCondition(borderCondId)->doCalc(mesh->get_current_time(), cur_node,
                                                                 new_node, cur_node.getRheologyMatrix(), previous_nodes, inner, outer_normal);
            }
                // Contact
            else {
                THROW_UNSUPPORTED("Contact conditions are not supported yet");
                //                CalcNode& virt_node = engine->getVirtNode(cur_node.contactNodeNum);
                //
                //
                //                LOG_TRACE("We are going to calc contact. Target virt node: "
                //                        << cur_node.contactNodeNum << " Target mesh: " << virt_node.contactNodeNum );
                //
                //                LOG_TRACE( "Mesh: " << mesh->getId()
                //                        << " Virt mesh: " << engine->getBody(virt_node.contactNodeNum)->getMeshes()->getId()
                //                        << "\nReal node: " << cur_node << "\nVirt node: " << virt_node);
                //
                //                // Mark virt node as having contact state
                //                // TODO FIXME - most probably CollisionDetector should do it
                //                // But we should check it anycase
                //                virt_node.setInContact(true);
                //                //virt_node.contactNodeNum = cur_node.contactNodeNum;
                //
                //                // Variables used in calculations internally
                //
                //                // Used for interpolated virtual node in case of contact algorithm
                //                // FIXME get rid of hardcoded type
                //                AnisotropicMatrix3D virt_rheologyMatrix;
                //
                //                // Delta x on previous time layer for all the omegas
                //                //     omega_new_time_layer(ksi) = omega_old_time_layer(ksi+dksi)
                //                float virt_dksi[9];
                //
                //                // If the corresponding point on previous time layer is inner or not
                //                bool virt_inner[9];
                //
                //                // We will store interpolated nodes on previous time layer here
                //                // We know that we need five nodes for each direction (corresponding to Lambdas -C1, -C2, 0, C2, C1)
                //                // TODO  - We can  deal with (lambda == 0) separately
                //                vector<CalcNode> virt_previous_nodes;
                //                virt_previous_nodes.resize(9);
                //
                //                // Outer normal at current point
                //                float virt_outer_normal[3];
                //
                //                // Number of outer characteristics
                //                LOG_TRACE("Start virt node calc");
                //                // FIXME - WA
                //                Mesh* virtMesh = (Mesh*) engine->getBody(virt_node.contactNodeNum)->getMeshes();
                //                int virt_outer_count = prepare_node( virt_node, virt_rheologyMatrix,
                //                        time_step, stage, virtMesh,
                //                        virt_dksi, virt_inner, virt_previous_nodes,
                //                        virt_outer_normal);
                //
                //                // TODO - merge this condition with the next ones
                //                if( virt_outer_count != 3 ) {
                //                    LOG_DEBUG("There are " << virt_outer_count << " 'outer' characteristics for virt node.");
                //                    LOG_DEBUG("Origin node: " << engine->getBody(virt_node.contactNodeNum)->getMeshes()->getNode( virt_node.number ) );
                //                    for(int z = 0; z < 9; z++)
                //                    {
                //                        LOG_DEBUG("Dksi[" << z << "]: " << virt_dksi[z]);
                //                        LOG_DEBUG("Inner[" << z << "]: " << virt_inner[z]);
                //                        LOG_DEBUG("PrNodes[" << z << "]: " << virt_previous_nodes[z]);
                //                    }
                //                    THROW_BAD_METHOD("Illegal number of outer characteristics");
                //                }
                //
                //                // Check that 'paired node' is in the direction of 'outer' characteristics
                //                // If it is not the case - we have strange situation when
                //                // we replace 'outer' points data with data of 'paired node' from different axis direction.
                //
                //                // For all characteristics of real node and virt node
                //                /*for(int i = 0; i < 9; i++)
                //                {
                //                    float v_x_outer[3];
                //                    float v_x_virt[3];
                //                    // Real node - if characteristic is 'outer'*/
                //    /*                if(!inner[i])
                //                    {
                //                        // Find directions to corresponding 'outer' point and to virt 'paired node'
                //                        for(int j = 0; j < 3; j++) {
                //                            v_x_outer[j] = previous_nodes[ppoint_num[i]].coords[j] - cur_node.coords[j];
                //                            v_x_virt[j] = virt_node.coords[j] - cur_node.coords[j];
                //                        }
                //                        // If directions are different - smth bad happens
                //                        if( (v_x_outer[0] * v_x_virt[0]
                //                             + v_x_outer[1] * v_x_virt[1] + v_x_outer[2] * v_x_virt[2]) < 0 )
                //                        {
                //                            *logger << "MESH " << mesh->zone_num << "REAL NODE " << cur_node.local_num << ": "
                //                                    << "x: " << cur_node.coords[0]
                //                                    << " y: " << cur_node.coords[1]
                //                                    << " z: " < cur_node.coords[2];
                //                            log_node_diagnostics(cur_node, stage, outer_normal, mesh, basis_num, rheologyMatrix, time_step, previous_nodes, ppoint_num, inner, dksi);
                //                            *logger << "'Outer' direction: " << v_x_outer[0] << " "
                //                                << v_x_outer[1] << " " < v_x_outer[2];
                //                            *logger << "'Virt' direction: " << v_x_virt[0] << " "
                //                                << v_x_virt[1] << " " < v_x_virt[2];
                //                            throw GCMException( GCMException::METHOD_EXCEPTION, "Bad contact from real node point of view: 'outer' and 'virt' directions are different");
                //                        }
                //                    }*/
                //    // We switch it off because it conflicts sometimes with 'safe_direction'
                //    /*                // Virt node - if characteristic is 'outer'
                //                    if(!virt_inner[i])
                //                    {
                //                        // Find directions to corresponding 'outer' point and to real 'paired node'
                //                        for(int j = 0; j < 3; j++) {
                //                            v_x_outer[j] = virt_previous_nodes[virt_ppoint_num[i]].coords[j] - virt_node.coords[j];
                //                            v_x_virt[j] = cur_node.coords[j] - virt_node.coords[j];
                //                        }
                //                        // If directions are different - smth bad happens
                //                        if( (v_x_outer[0] * v_x_virt[0]
                //                            + v_x_outer[1] * v_x_virt[1] + v_x_outer[2] * v_x_virt[2]) < 0 )
                //                        {
                //                            *logger << "MESH " << mesh->zone_num << "REAL NODE " << cur_node.local_num << ": "
                //                                    << "x: " << cur_node.coords[0]
                //                                    << " y: " << cur_node.coords[1]
                //                                    << " z: " < cur_node.coords[2];
                //                            log_node_diagnostics(virt_node, stage, virt_outer_normal, virt_node.mesh, basis_num, virt_rheologyMatrix, time_step, virt_previous_nodes, virt_ppoint_num, virt_inner, virt_dksi);
                //                            *logger << "'Outer' direction: " << v_x_outer[0] << " "
                //                                << v_x_outer[1] << " "< v_x_outer[2];
                //                            *logger << "'Virt' direction: " << v_x_virt[0] << " "
                //                                << v_x_virt[1] << " " < v_x_virt[2];
                //                            throw GCMException( GCMException::METHOD_EXCEPTION, "Bad contact from virt node point of view: 'outer' and 'virt' directions are different");
                //                        }
                //                    }*/
                ////                }
                //
                //                LOG_TRACE("Using calculator: " << engine->getContactCondition(0)->calc->getType());
                //                engine->getContactCondition(0)->doCalc(mesh->get_current_time(), cur_node,
                //                        new_node, virt_node, rheologyMatrix, previous_nodes, inner,
                //                        virt_rheologyMatrix, virt_previous_nodes, virt_inner, outer_normal);
            }
            // It means smth went wrong. Just interpolate the values and report bad node.
        }
        else {
            // FIXME - implement border and contact completely
            LOG_TRACE("Using calculator: " << engine->getBorderCondition(0)->calc->getType());
            engine->getBorderCondition(0)->doCalc(mesh->get_current_time(), cur_node,
                                                  new_node, cur_node.getRheologyMatrix(), previous_nodes, inner, outer_normal);
            cur_node.setNeighError(stage);
        }
        LOG_TRACE("Done border node calc");
    }
}

int gcm::InterpolationFixedAxis::prepare_node(CalcNode& cur_node, RheologyMatrixPtr rheologyMatrix,
                                              float time_step, int stage, Mesh* mesh,
                                              float* dksi, bool* inner, vector<CalcNode>& previous_nodes,
                                              float* outer_normal)
{
    assert_ge(stage, 0);
    assert_le(stage, 2);

    if (cur_node.isBorder())
        mesh->findBorderNodeNormal(cur_node.number, &outer_normal[0], &outer_normal[1], &outer_normal[2], false);

    LOG_TRACE("Preparing elastic matrix");
    //  Prepare matrixes  A, Lambda, Omega, Omega^(-1)

    switch (stage) {
    case 0: rheologyMatrix->decomposeX(cur_node);
        break;
    case 1: rheologyMatrix->decomposeY(cur_node);
        break;
    case 2: rheologyMatrix->decomposeZ(cur_node);
        break;
    }
    LOG_TRACE("Preparing elastic matrix done");

    LOG_TRACE("Elastic matrix eigen values:\n" << rheologyMatrix->getL());

    for (int i = 0; i < 9; i++)
        dksi[i] = -rheologyMatrix->getL(i, i) * time_step;

    return find_nodes_on_previous_time_layer(cur_node, stage, mesh, dksi, inner, previous_nodes, outer_normal);
}

int gcm::InterpolationFixedAxis::find_nodes_on_previous_time_layer(CalcNode& cur_node, int stage, Mesh* mesh,
                                                                   float dksi[], bool inner[], vector<CalcNode>& previous_nodes,
                                                                   float outer_normal[])
{
    LOG_TRACE("Start looking for nodes on previous time layer");

    // For all omegas
    for (int i = 0; i < 9; i++) {
        LOG_TRACE("Looking for characteristic " << i);
        // Check prevoius omegas ...
        bool already_found = false;
        for (int j = 0; j < i; j++) {
            // ... And try to find if we have already worked with the required point
            // on previous time layer (or at least with the point that is close enough)
            if (fabs(dksi[i] - dksi[j]) <= EQUALITY_TOLERANCE * 0.5 * fabs(dksi[i] + dksi[j])) {
                LOG_TRACE("Found old value " << dksi[i] << " - done");
                // If we have already worked with this point - just remember the number
                already_found = true;
                previous_nodes[i] = previous_nodes[j];
                inner[i] = inner[j];
            }
        }

        // If we do not have necessary point in place - ...
        if (!already_found) {
            LOG_TRACE("New value " << dksi[i] << " - preparing vectors");
            // ... Put new number ...
            previous_nodes[i] = cur_node;

            // ... Find vectors ...
            float dx[3];
            // WA:
            //     origin == cur_node for real nodes
            //     origin != cure_node for virt nodes
            CalcNode& origin = mesh->getNode(cur_node.number);
            for (int z = 0; z < 3; z++) {
                dx[z] = cur_node.coords[z] - origin.coords[z];
            }
            dx[stage] += dksi[i];

            // For dksi = 0 we can skip check and just copy everything
            if (dksi[i] == 0) {
                // no interpolation required - everything is already in place
                inner[i] = true;
                LOG_TRACE("dksi is zero - done");
            }
            else if (cur_node.isInner()) {
                LOG_TRACE("Checking inner node");
                // ... Find owner tetrahedron ...
                bool isInnerPoint;
                mesh->interpolateNode(origin, dx[0], dx[1], dx[2], false,
                                      previous_nodes[i], isInnerPoint);

                if (!isInnerPoint) {
                    LOG_TRACE("Inner node: we need new method here!");
                    LOG_TRACE("Node:\n" << cur_node);
                    LOG_TRACE("Move: " << dx[0] << " " << dx[1] << " " << dx[2]);
                    // Re-run search with debug on
                    mesh->interpolateNode(origin, dx[0], dx[1], dx[2], true,
                                          previous_nodes[i], isInnerPoint);
                }

                inner[i] = true;
                LOG_TRACE("Checking inner node done");
            }
            else if (cur_node.isBorder()) {
                LOG_TRACE("Checking border node");
                // ... Find owner tetrahedron ...
                bool isInnerPoint;
                bool interpolated = mesh->interpolateNode(origin, dx[0], dx[1], dx[2], false,
                                                          previous_nodes[i], isInnerPoint);

                // If we found inner point, it means
                // this direction is inner and everything works as for usual inner point
                if (isInnerPoint) {
                    inner[i] = true;
                    // If we did not find inner point - two cases are possible
                }
                else {
                    inner[i] = false;
                    // We found border cross somehow
                    // It can happen if we work with really thin structures and big time step
                    // We can work as usual in this case
                    if (interpolated) {
                        LOG_TRACE("Border node: we need new method here!");
                        inner[i] = true;
                        // Or we did not find any point at all - it means this characteristic is outer
                    }
                    else {
                        inner[i] = false;
                    }
                }
                LOG_TRACE("Checking border node done");
            }
            else {
                THROW_BAD_MESH("Unsupported case for characteristic location");
            }
        }
        LOG_TRACE("Looking for characteristic " << i << " done");
    }

    int outer_count = 0;
    for (int i = 0; i < 9; i++)
        if (!inner[i])
            outer_count++;

    // assert_true(outer_count == 0 || outer_count == 3);

    LOG_TRACE("Looking for nodes on previous time layer done. Outer count = " << outer_count);

    return outer_count;
}
