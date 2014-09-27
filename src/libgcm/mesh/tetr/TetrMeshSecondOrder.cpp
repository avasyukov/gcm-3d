#include "libgcm/mesh/tetr/TetrMeshSecondOrder.hpp"

#include "libgcm/snapshot/VTK2SnapshotWriter.hpp"
#include "libgcm/snapshot/VTKSnapshotWriter.hpp"

#include "libgcm/node/CalcNode.hpp"

using namespace gcm;
using std::unordered_map;
using std::make_pair;
using std::vector;

TetrMeshSecondOrder::TetrMeshSecondOrder()
{
    secondOrderNodesAreGenerated = false;
    numericalMethodOrder = 2;
    INIT_LOGGER("gcm.TetrMeshSecondOrder");
    interpolator = new TetrSecondOrderMinMaxInterpolator();
}

TetrMeshSecondOrder::~TetrMeshSecondOrder()
{
    delete interpolator;
}

void TetrMeshSecondOrder::createTetrs(int number)
{
    LOG_DEBUG("Creating second order tetrs storage, size: " << (int) (number * STORAGE_OVERCOMMIT_RATIO));
    tetrs2.resize(number * STORAGE_OVERCOMMIT_RATIO);
    tetrsStorageSize = number*STORAGE_OVERCOMMIT_RATIO;
}

void TetrMeshSecondOrder::createTriangles(int number)
{
    LOG_DEBUG("Creating second order border storage, size: " << number);
    // TODO - make border working through addTriangle() / faceNumber++ / etc
    border2.resize(number);
    faceNumber = number;
    faceStorageSize = number;
}

TetrFirstOrder& TetrMeshSecondOrder::getTetr(unsigned int index)
{
    assert_ge(index, 0);
    unordered_map<int, int>::const_iterator itr;
    itr = tetrsMap.find(index);
    assert_true(itr != tetrsMap.end());
    return tetrs2[itr->second];
}

TetrSecondOrder& TetrMeshSecondOrder::getTetr2(int index)
{
    assert_ge(index, 0);
    unordered_map<int, int>::const_iterator itr;
    itr = tetrsMap.find(index);
    assert_true(itr != tetrsMap.end());
    return tetrs2[itr->second];
}

TetrFirstOrder& TetrMeshSecondOrder::getTetrByLocalIndex(unsigned int index)
{
    assert_ge(index, 0);
    return tetrs2[index];
}

TetrSecondOrder& TetrMeshSecondOrder::getTetr2ByLocalIndex(int index)
{
    assert_ge(index, 0);
    return tetrs2[index];
}

void TetrMeshSecondOrder::rebuildMaps()
{
    nodesMap.clear();
    for (int i = 0; i < nodesNumber; i++)
        nodesMap[getNodeByLocalIndex(i).number] = i;
    tetrsMap.clear();
    for (int i = 0; i < tetrsNumber; i++)
        tetrsMap[getTetr2ByLocalIndex(i).number] = i;
}

void TetrMeshSecondOrder::addTetr(TetrFirstOrder& tetr)
{
    if (tetrsNumber == tetrsStorageSize)
        createTetrs(tetrsStorageSize * STORAGE_ONDEMAND_GROW_RATE);
    assert_lt(tetrsNumber, tetrsStorageSize);
    tetrs2[tetrsNumber].number = tetr.number;
    memcpy(tetrs2[tetrsNumber].verts, tetr.verts, 4 * sizeof (int));
    tetrsMap[tetr.number] = tetrsNumber;
    tetrsNumber++;
}

void TetrMeshSecondOrder::addTetr2(TetrSecondOrder& tetr)
{
    if (tetrsNumber == tetrsStorageSize)
        createTetrs(tetrsStorageSize * STORAGE_ONDEMAND_GROW_RATE);
    assert_lt(tetrsNumber, tetrsStorageSize);
    tetrs2[tetrsNumber] = tetr;
    tetrsMap[tetr.number] = tetrsNumber;
    tetrsNumber++;
}

TriangleFirstOrder& TetrMeshSecondOrder::getTriangle(int index)
{
    assert_ge(index, 0);
    return border2[index];
}

TriangleSecondOrder& TetrMeshSecondOrder::getTriangle2(int index)
{
    assert_ge(index, 0);
    return border2[index];
}

void TetrMeshSecondOrder::copyMesh(TetrMeshFirstOrder* src)
{
    LOG_DEBUG("Creating mesh using copy");
    firstOrderNodesNumber = src->getNodesNumber();
    secondOrderNodesNumber = countSecondOrderNodes(src);

    LOG_DEBUG("Copying first order nodes");

    createNodes(firstOrderNodesNumber + secondOrderNodesNumber);
    for (int i = 0; i < firstOrderNodesNumber; i++)
        addNode(src->getNodeByLocalIndex(i));

    LOG_DEBUG("Copying first order tetrs");

    createTetrs(src->getTetrsNumber());
    //for( int i = 0; i < tetrsNumber; i++ )
    //for( MapIter itr = tetrsMap.begin(); itr != tetrsMap.end(); ++itr ) {
    //    int i = itr->first;
    //    addTetr( src->getTetr(i) );
    //}
    for (int i = 0; i < src->getTetrsNumber(); i++) {
        addTetr(src->getTetrByLocalIndex(i));
    }

    LOG_DEBUG("Generating second order nodes");
    generateSecondOrderNodes();
}

void TetrMeshSecondOrder::copyMesh2(TetrMeshSecondOrder* src)
{
    LOG_DEBUG("Creating second order mesh using copy");

    LOG_DEBUG("Nodes: " << src->getNodesNumber());
    createNodes(src->getNodesNumber());
    for (int i = 0; i < src->getNodesNumber(); i++)
        addNode(src->getNodeByLocalIndex(i));

    LOG_DEBUG("Tetrs: " << src->getTetrsNumber());
    createTetrs(src->getTetrsNumber());
    //for( int i = 0; i < tetrsNumber; i++ )
    for (MapIter itr = tetrsMap.begin(); itr != tetrsMap.end(); ++itr) {
        int i = itr->first;
        addTetr2(src->getTetr2(i));
    }
    for (int i = 0; i < src->getTetrsNumber(); i++) {
        addTetr2(src->getTetr2ByLocalIndex(i));
    }
}

void TetrMeshSecondOrder::preProcessGeometry()
{
    LOG_DEBUG("Preprocessing second order mesh started");

    calcMaxH();
    calcAvgH();
    mesh_min_h *= 0.5;
    mesh_max_h *= 0.5;
    mesh_avg_h *= 0.5;

    verifyTetrahedraVertices();
    build_volume_reverse_lookups();

    check_unused_nodes();
    build_first_order_border();
    generateSecondOrderBorder();
    build_surface_reverse_lookups();

    check_numbering();
    check_outer_normals();
    LOG_DEBUG("Preprocessing mesh done.");
}

void TetrMeshSecondOrder::verifyTetrahedraVertices()
{
    LOG_DEBUG("Verifying second order tetrahedra vertices");
    for (int tetrInd = 0; tetrInd < tetrsNumber; tetrInd++) {
        TetrSecondOrder& tetr = getTetr2ByLocalIndex(tetrInd);
        for (int vertInd = 0; vertInd < 4; vertInd++) {
            int nodeNum = tetr.verts[vertInd];
            assert_ge(nodeNum, 0);
            assert_true(getNode(nodeNum).isFirstOrder());
        }

        for (int addVertInd = 0; addVertInd < 6; addVertInd++) {
            int addNodeNum = tetr.addVerts[addVertInd];
            assert_ge(addNodeNum, 0);
            assert_true(getNode(addNodeNum).isSecondOrder());
        }
    }
}

void TetrMeshSecondOrder::build_volume_reverse_lookups()
{
    LOG_DEBUG("Building volume reverse lookups for second order mesh");

    // Init vectors for "reverse lookups" of tetrahedrons current node is a member of.
    //for(int i = 0; i < nodesNumber; i++) {
    for (MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr) {
        int i = itr->first;
        getVolumeElementsForNode(i).clear();
    }

    // Go through all the tetrahedrons
    //for(int i = 0; i < tetrsNumber; i++) {
    for (MapIter itr = tetrsMap.begin(); itr != tetrsMap.end(); ++itr) {
        int i = itr->first;
        // For all verticles
        for (int j = 0; j < 4; j++) {
            TetrFirstOrder& tetr = getTetr(i);
            int nodeInd = tetr.verts[j];
            assert_true(getNode(nodeInd).isFirstOrder());
            // Push to data of nodes the number of this tetrahedron
            getVolumeElementsForNode(nodeInd).push_back(tetr.number);
        }
        for (int j = 0; j < 6; j++) {
            TetrSecondOrder& tetr2 = getTetr2(i);
            int nodeInd = tetr2.addVerts[j];
            assert_true(getNode(nodeInd).isSecondOrder());
            // Push to data of nodes the number of this tetrahedron
            getVolumeElementsForNode(nodeInd).push_back(tetr2.number);
        }
    }

    for (MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr) {
        CalcNode& node = getNode(itr->first);
        int num = getVolumeElementsForNode(itr->first).size();
        if (num <= 0)
            LOG_DEBUG("Node is not a part of volumes. Node: " << node);
    }
}

void TetrMeshSecondOrder::build_first_order_border()
{
    // Prepare border data

    float solid_angle;
    float solid_angle_part;

    LOG_DEBUG("Looking for border nodes using angles");
    int nodeCount = 0;

    // Check border using solid angle comparation with 4*PI

    //for( int i = 0; i < nodesNumber; i++ ) {
    for (MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr) {
        int i = itr->first;
        CalcNode& node = getNode(i);
        node.setIsBorder(false);
        if (/*node.isLocal() &&*/ node.isFirstOrder()) {
            solid_angle = 0;
            vector<int>& elements = getVolumeElementsForNode(i);
            for (unsigned j = 0; j < elements.size(); j++) {
                solid_angle_part = get_solid_angle(i, elements[j]);
                if (solid_angle_part < 0)
                    for (unsigned z = 0; z < elements.size(); z++)
                        LOG_DEBUG("Element: " << elements[z]);
                assert_ge(solid_angle_part, 0);
                solid_angle += solid_angle_part;
            }
            if (fabs(4 * M_PI - solid_angle) > M_PI * EQUALITY_TOLERANCE) {
                node.setIsBorder(true);
                nodeCount++;
            }
        }
    }
    LOG_DEBUG("Found " << nodeCount << " border nodes");

    LOG_DEBUG("Constructing border triangles");

    int faceCount = 0;
    int number = 0;

    // FIXME TODO - make faces uniq!

    // Check all tetrs and construct border triangles
    //for(int i = 0; i < tetrsNumber; i++) {
    for (MapIter itr = tetrsMap.begin(); itr != tetrsMap.end(); ++itr) {
        int i = itr->first;
        for (int j = 0; j < 4; j++) {
            if (isTriangleBorder(getTetr(i).verts))
                faceCount++;
            shiftArrayLeft(getTetr(i).verts, 4);
        }
    }

    LOG_DEBUG("Found " << faceCount << " border triangles");

    createTriangles(faceCount);

    LOG_DEBUG("Triangles storage allocated");
    
    // WA for bruteforce collision detector
    createOutline();

    //for(int i = 0; i < tetrsNumber; i++) {
    for (MapIter itr = tetrsMap.begin(); itr != tetrsMap.end(); ++itr) {
        int i = itr->first;
        for (int j = 0; j < 4; j++) {
            if (isTriangleBorder(getTetr(i).verts)) {
                getTriangle(number) = createBorderTriangle(getTetr(i).verts, number);
                number++;
                // WA for bruteforce collision detector
                TetrFirstOrder& t = getTetr(i);
                CalcNode& v1 = getNode( t.verts[0] );
                CalcNode& v2 = getNode( t.verts[1] );
                CalcNode& v3 = getNode( t.verts[2] );
                real minX = std::min({v1.coords.x, v2.coords.x, v3.coords.x});
                real minY = std::min({v1.coords.y, v2.coords.y, v3.coords.y});
                real minZ = std::min({v1.coords.z, v2.coords.z, v3.coords.z});
                real maxX = std::max({v1.coords.x, v2.coords.x, v3.coords.x});
                real maxY = std::max({v1.coords.y, v2.coords.y, v3.coords.y});
                real maxZ = std::max({v1.coords.z, v2.coords.z, v3.coords.z});
                real xh = (outline.maxX - outline.minX)/FACES_SPACE_MAP_SIZE;
                real yh = (outline.maxY - outline.minY)/FACES_SPACE_MAP_SIZE;
                real zh = (outline.maxZ - outline.minZ)/FACES_SPACE_MAP_SIZE;
                int minZoneX = std::min(0, (int)floor((minX - outline.minX)/xh));
                int minZoneY = std::min(0, (int)floor((minY - outline.minY)/yh));
                int minZoneZ = std::min(0, (int)floor((minZ - outline.minZ)/zh));
                int maxZoneX = std::max(FACES_SPACE_MAP_SIZE-1, (int)floor((maxX - outline.minX)/xh));
                int maxZoneY = std::max(FACES_SPACE_MAP_SIZE-1, (int)floor((maxY - outline.minY)/yh));
                int maxZoneZ = std::max(FACES_SPACE_MAP_SIZE-1, (int)floor((maxZ - outline.minZ)/zh));
                minZoneX = (minZoneX >= 0 ? minZoneX : 0);
                minZoneX = (minZoneX <= FACES_SPACE_MAP_SIZE-1 ? minZoneX : FACES_SPACE_MAP_SIZE-1);
                minZoneY = (minZoneY >= 0 ? minZoneY : 0);
                minZoneY = (minZoneY <= FACES_SPACE_MAP_SIZE-1 ? minZoneY : FACES_SPACE_MAP_SIZE-1);
                minZoneZ = (minZoneZ >= 0 ? minZoneZ : 0);
                minZoneZ = (minZoneZ <= FACES_SPACE_MAP_SIZE-1 ? minZoneZ : FACES_SPACE_MAP_SIZE-1);
                maxZoneX = (maxZoneX >= 0 ? maxZoneX : 0);
                maxZoneX = (maxZoneX <= FACES_SPACE_MAP_SIZE-1 ? maxZoneX : FACES_SPACE_MAP_SIZE-1);
                maxZoneY = (maxZoneY >= 0 ? maxZoneY : 0);
                maxZoneY = (maxZoneY <= FACES_SPACE_MAP_SIZE-1 ? maxZoneY : FACES_SPACE_MAP_SIZE-1);
                maxZoneZ = (maxZoneZ >= 0 ? maxZoneZ : 0);
                maxZoneZ = (maxZoneZ <= FACES_SPACE_MAP_SIZE-1 ? maxZoneZ : FACES_SPACE_MAP_SIZE-1);
                for(int xi = minZoneX; xi <= maxZoneX; xi++)
                    for(int yi = minZoneY; yi <= maxZoneY; yi++)
                        for(int zi = minZoneZ; zi <= maxZoneZ; zi++)
                            facesSpaceMap[xi][yi][zi].push_back(number-1);
                assert_eq(number, facesSpaceMap[0][0][0].size());
            }
            shiftArrayLeft(getTetr(i).verts, 4);
        }
    }
    
    //if( number != faceCount )
    //    LOG_WARN("Number: " << number << " FaceCount: " << faceCount);
    assert_eq(number, faceCount);
    LOG_DEBUG("Created " << faceNumber << " triangles");
}

void TetrMeshSecondOrder::generateSecondOrderBorder()
{
    LOG_DEBUG("Faces: " << faceNumber);
    bool debug = false;

    LOG_DEBUG("Generating second order border");
    IntPair combinations[3];
    combinations[0] = make_pair(0, 1);
    combinations[1] = make_pair(0, 2);
    combinations[2] = make_pair(1, 2);

    int v1, v2, ind;
    int v1pos, v2pos, l;
    int curTInd;
    int minI, maxI;
    for (int i = 0; i < faceNumber; i++) {
        for (int j = 0; j < 3; j++) {
            v1 = getTriangle(i).verts[ combinations[j].first ];
            v2 = getTriangle(i).verts[ combinations[j].second ];

            //if( ( v1 == 0 && v2 == 420 ) || ( v1 == 420 && v2 == 0 ) )
            //    debug = true;

            ind = -1;
            vector<int>& elements = getVolumeElementsForNode(v1);
            l = elements.size();
            for (int k = 0; k < l; k++) {
                v1pos = -1;
                v2pos = -1;
                curTInd = elements[k];
                for (int z = 0; z < 4; z++) {
                    if (getTetr(curTInd).verts[z] == v1)
                        v1pos = z;
                    if (getTetr(curTInd).verts[z] == v2)
                        v2pos = z;
                }
                if (v1pos >= 0 && v2pos >= 0) {
                    minI = (v1pos > v2pos ? v2pos : v1pos);
                    maxI = (v1pos > v2pos ? v1pos : v2pos);
                    if (minI == 0 && maxI == 1)
                        ind = getTetr2(curTInd).addVerts[0];
                    else if (minI == 0 && maxI == 2)
                        ind = getTetr2(curTInd).addVerts[1];
                    else if (minI == 0 && maxI == 3)
                        ind = getTetr2(curTInd).addVerts[2];
                    else if (minI == 1 && maxI == 2)
                        ind = getTetr2(curTInd).addVerts[3];
                    else if (minI == 1 && maxI == 3)
                        ind = getTetr2(curTInd).addVerts[4];
                    else if (minI == 2 && maxI == 3)
                        ind = getTetr2(curTInd).addVerts[5];
                    if (debug) {
                        LOG_DEBUG("Tetr: " << curTInd << " Verts: " << v1 << " " << v2);
                        LOG_DEBUG("Found origin for test node");
                        LOG_DEBUG("Positions: " << v1pos << " " << v2pos);
                        LOG_DEBUG("Index: " << ind);
                        LOG_DEBUG("Node: " << getNode(ind));
                    }
                    break;
                }
            }

            assert_ne(ind, -1);
            assert_true(getNode(ind).isSecondOrder());
            getTriangle2(i).addVerts[j] = ind;
            getNode(ind).setIsBorder(true);
        }
    }

    LOG_DEBUG("Second order border generated");
}

void TetrMeshSecondOrder::build_surface_reverse_lookups()
{
    LOG_DEBUG("Building surface reverse lookups for second order mesh");

    // Init vectors for "reverse lookups" of border triangles current node is a member of.
    //for(int i = 0; i < nodesNumber; i++) {
    for (MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr) {
        int i = itr->first;
        getBorderElementsForNode(i).clear();
    }

    // Go through all the triangles and push to data of nodes the number of this triangle
    for (int i = 0; i < faceNumber; i++) {
        for (int j = 0; j < 3; j++) {
            int nodeInd = getTriangle(i).verts[j];
            assert_true(getNode(nodeInd).isFirstOrder());
            getBorderElementsForNode(nodeInd).push_back(i);
        }
        for (int j = 0; j < 3; j++) {
            int nodeInd = getTriangle2(i).addVerts[j];
            assert_true(getNode(nodeInd).isSecondOrder());
            getBorderElementsForNode(nodeInd).push_back(i);
        }
    }

    for (MapIter itr = nodesMap.begin(); itr != nodesMap.end(); ++itr) {
        CalcNode& node = getNode(itr->first);
        int num = getBorderElementsForNode(itr->first).size();
        if (node.isBorder() && num <= 0)
            LOG_DEBUG("Border node is not a part of faces. Node: " << node);
    }
}

void TetrMeshSecondOrder::moveCoords(float tau)
{
    // Move first order nodes
    TetrMeshFirstOrder::moveCoords(tau);
    mesh_min_h *= 0.5;

    // Move second order nodes
    IntPair combinations[6];
    combinations[0] = make_pair(0, 1);
    combinations[1] = make_pair(0, 2);
    combinations[2] = make_pair(0, 3);
    combinations[3] = make_pair(1, 2);
    combinations[4] = make_pair(1, 3);
    combinations[5] = make_pair(2, 3);

    int v1, v2, ind;
    for (MapIter itr = tetrsMap.begin(); itr != tetrsMap.end(); ++itr) {
        int i = itr->first;
        for (int j = 0; j < 6; j++) {
            ind = getTetr2(i).addVerts[j];
            v1 = getTetr(i).verts[ combinations[j].first ];
            v2 = getTetr(i).verts[ combinations[j].second ];
            moveSecondOrderNode(ind, v1, v2);
        }
    }
}

void TetrMeshSecondOrder::moveSecondOrderNode(int nodeIdx, int nodeIdx1, int nodeIdx2)
{
    CalcNode& node = getNode(nodeIdx);
    CalcNode& newNode = getNewNode(nodeIdx);
    CalcNode& node1 = getNode(nodeIdx1);
    CalcNode& node2 = getNode(nodeIdx2);

    for (int i = 0; i < 3; i++) {
        node.coords[i] = (node1.coords[i] + node2.coords[i]) * 0.5;
        newNode.coords[i] = node.coords[i];
    }
}

void TetrMeshSecondOrder::fillSecondOrderNode(CalcNode& newNode, int nodeIdx1, int nodeIdx2)
{
    CalcNode& node1 = getNode(nodeIdx1);
    CalcNode& node2 = getNode(nodeIdx2);

    for (int i = 0; i < 3; i++)
        newNode.coords[i] = (node1.coords[i] + node2.coords[i]) * 0.5;

    for (int i = 0; i < 9; i++)
        newNode.values[i] = (node1.values[i] + node2.values[i]) * 0.5;

    newNode.setRho((node1.getRho() + node2.getRho()) * 0.5);
    newNode.setMaterialId(node1.getMaterialId());

    newNode.setPlacement(true);
    newNode.setOrder(2);
}

int TetrMeshSecondOrder::countSecondOrderNodes(TetrMeshFirstOrder* src)
{
    assert_true(src);
    LOG_DEBUG("Counting additional nodes");

    int firstOrderNodesCount = src->getNodesNumber();

    IntPair combinations[6];
    combinations[0] = make_pair(0, 1);
    combinations[1] = make_pair(0, 2);
    combinations[2] = make_pair(0, 3);
    combinations[3] = make_pair(1, 2);
    combinations[4] = make_pair(1, 3);
    combinations[5] = make_pair(2, 3);

    int minFirstOrderNum = src->getNodeByLocalIndex(0).number;
    int maxFirstOrderNum = src->getNodeByLocalIndex(0).number;
    for (int i = 0; i < firstOrderNodesCount; i++) {
        int num = src->getNodeByLocalIndex(i).number;
        if (num > maxFirstOrderNum)
            maxFirstOrderNum = num;
        if (num < minFirstOrderNum)
            minFirstOrderNum = num;
    }
    int firstOrderLength = maxFirstOrderNum - minFirstOrderNum + 1;
    //LOG_DEBUG("Min: " << minFirstOrderNum << " Max: " << maxFirstOrderNum << " Len: " << firstOrderLength);

    vector<IntPair>** processed = new vector<IntPair>*[firstOrderLength];
    for (int i = 0; i < firstOrderLength; i++)
        processed[i] = new vector<IntPair>;

    int secondOrderNodesCount = 0;
    int v1, v2;
    int ind;
    for (int i = 0; i < src->getTetrsNumber(); i++) {
        TetrFirstOrder& tetr = src->getTetrByLocalIndex(i);
        //if( body->getEngine()->getRank() == 1 )
        //    LOG_DEBUG("Tetr " << i << " Num: " << tetr.number);
        for (int j = 0; j < 6; j++) {
            v1 = tetr.verts[ combinations[j].first ];
            v2 = tetr.verts[ combinations[j].second ];
            //if( body->getEngine()->getRank() == 1 )
            //    LOG_DEBUG("V1 = " << v1 << " V2 = " << v2);
            ind = -1;

            for (unsigned int z = 0; z < processed[v1 - minFirstOrderNum]->size(); z++)
                if ((processed[v1 - minFirstOrderNum]->at(z)).second == v2)
                    ind = (processed[v1 - minFirstOrderNum]->at(z)).first;

            if (ind == -1) {
                IntPair in;
                in.first = firstOrderNodesCount + secondOrderNodesCount;
                in.second = v2;
                processed[v1 - minFirstOrderNum]->push_back(in);
                in.second = v1;
                processed[v2 - minFirstOrderNum]->push_back(in);
                secondOrderNodesCount++;
            }
        }
    }

    for (int i = 0; i < firstOrderLength; i++)
        delete processed[i];
    delete[] processed;

    LOG_DEBUG("We are going to create " << secondOrderNodesCount << " second order nodes");

    return secondOrderNodesCount;
}

void TetrMeshSecondOrder::generateSecondOrderNodes()
{
    CalcNode node;

    if (secondOrderNodesAreGenerated)
        return;

    int minNodeNum = getNodeByLocalIndex(0).number;

    for (int i = 0; i < firstOrderNodesNumber; i++) {
        getNodeByLocalIndex(i).setOrder(1);
        if (getNodeByLocalIndex(i).number < minNodeNum)
            minNodeNum = getNodeByLocalIndex(i).number;
    }

    LOG_DEBUG("Creating second order nodes");
    LOG_DEBUG("Number of first order nodes: " << nodesNumber);
    assert_eq(firstOrderNodesNumber, nodesNumber);

    IntPair combinations[6];
    combinations[0] = make_pair(0, 1);
    combinations[1] = make_pair(0, 2);
    combinations[2] = make_pair(0, 3);
    combinations[3] = make_pair(1, 2);
    combinations[4] = make_pair(1, 3);
    combinations[5] = make_pair(2, 3);

    vector<IntPair>** processed = new vector<IntPair>*[minNodeNum + firstOrderNodesNumber];
    for (int i = 0; i < minNodeNum + firstOrderNodesNumber; i++)
        processed[i] = new vector<IntPair>;

    int secondOrderNodesCount = 0;
    int v1, v2;
    int ind;
    //for( int i = 0; i < tetrsNumber; i++) {
    for (MapIter itr = tetrsMap.begin(); itr != tetrsMap.end(); ++itr) {
        int i = itr->first;
        for (int j = 0; j < 6; j++) {
            v1 = getTetr(i).verts[ combinations[j].first ];
            v2 = getTetr(i).verts[ combinations[j].second ];
            ind = -1;

            for (unsigned int z = 0; z < processed[v1]->size(); z++)
                if ((processed[v1]->at(z)).second == v2)
                    ind = (processed[v1]->at(z)).first;

            if (ind == -1) {
                ind = minNodeNum + firstOrderNodesNumber + secondOrderNodesCount;
                fillSecondOrderNode(node, v1, v2);
                node.number = ind;
                addNode(node);
                IntPair in;
                in.first = ind;
                in.second = v2;
                processed[v1]->push_back(in);
                in.second = v1;
                processed[v2]->push_back(in);
                secondOrderNodesCount++;
            }
            getTetr2(i).addVerts[j] = ind;
        }
    }

    for (int i = 0; i < minNodeNum + firstOrderNodesNumber; i++)
        delete processed[i];
    delete[] processed;

    LOG_DEBUG("Second order nodes created");

    LOG_DEBUG("Total number of nodes: " << nodesNumber);

    secondOrderNodesAreGenerated = true;
}

bool TetrMeshSecondOrder::interpolateNode(CalcNode& origin, float dx, float dy, float dz, bool debug,
                                               CalcNode& targetNode, bool& isInnerPoint)
{
    int tetrInd = findOwnerTetr(origin, dx, dy, dz, debug,
                                targetNode.coords, &isInnerPoint);

    if (tetrInd == -1)
        return false;

    TetrSecondOrder& tmp_tetr = getTetr2(tetrInd);
    interpolator->interpolate(targetNode,
                              getNode(tmp_tetr.verts[0]), getNode(tmp_tetr.verts[1]),
                              getNode(tmp_tetr.verts[2]), getNode(tmp_tetr.verts[3]),
                              getNode(tmp_tetr.addVerts[0]), getNode(tmp_tetr.addVerts[1]),
                              getNode(tmp_tetr.addVerts[2]), getNode(tmp_tetr.addVerts[3]),
                              getNode(tmp_tetr.addVerts[4]), getNode(tmp_tetr.addVerts[5]));
    return true;
}

// FIXME_ASAP: rewrite it
bool TetrMeshSecondOrder::interpolateNode(CalcNode& node)
{
    for (int i = 0; i < getTetrsNumber(); i++)
    {
        TetrSecondOrder& t = getTetr2ByLocalIndex(i);
        if ( pointInTetr(node.coords.x, node.coords.y, node.coords.z,
                getNode(t.verts[0]).coords, getNode(t.verts[1]).coords,
                getNode(t.verts[2]).coords, getNode(t.verts[3]).coords, false) )
        {
            interpolator->interpolate( node,
                    getNode(t.verts[0]), getNode(t.verts[1]),
                    getNode(t.verts[2]), getNode(t.verts[3]),
                    getNode(t.addVerts[0]), getNode(t.addVerts[1]),
                    getNode(t.addVerts[2]), getNode(t.addVerts[3]),
                    getNode(t.addVerts[4]), getNode(t.addVerts[5]));
            return true;
        }
    }

    return false;
}


const SnapshotWriter& TetrMeshSecondOrder::getSnaphotter() const
{
    return VTKSnapshotWriter::getInstance();
}

const SnapshotWriter& TetrMeshSecondOrder::getDumper() const {
    return VTK2SnapshotWriter::getInstance();
}
