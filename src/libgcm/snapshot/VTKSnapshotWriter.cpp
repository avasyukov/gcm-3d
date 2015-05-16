#include "libgcm/snapshot/VTKSnapshotWriter.hpp"

#include "libgcm/mesh/tetr/TetrMeshFirstOrder.hpp"

#include <vtkTetra.h>

using namespace gcm;
using std::map;

template <>
MeshNodeIterator<TetrMeshFirstOrder, SNAPSHOTTER_ID_VTK>& MeshNodeIterator<TetrMeshFirstOrder, SNAPSHOTTER_ID_VTK>::operator++()
{
    index++;
    if (!hasNext())
        return *this;
    if (!mesh->getNodeByLocalIndex(index).isFirstOrder())
        return ++(*this);

    return *this;
}

VTKSnapshotWriter::VTKSnapshotWriter() {
    INIT_LOGGER("gcm.VTKSnapshotWriter");
    extension = "vtu";
}

void VTKSnapshotWriter::dumpMeshSpecificData(TetrMeshFirstOrder* mesh, vtkSmartPointer<vtkUnstructuredGrid>& grid, vtkSmartPointer<vtkPoints>& points) const
{
    map<int, int> snapNodeMap;
    int snapNodeCount = 0;

    for(int i = 0; i < mesh->getNodesNumber(); i++)
    {
        CalcNode& node = mesh->getNodeByLocalIndex(i);
        if(node.isFirstOrder())
        {
            snapNodeMap[node.number] = snapNodeCount++;
            points->InsertNextPoint( node.coords[0], node.coords[1], node.coords[2] );
        }
    }

    auto tetra=vtkSmartPointer<vtkTetra>::New();
    for(int i = 0; i < mesh->getTetrsNumber(); i++)
    {
        TetrFirstOrder& tetr = mesh->getTetrByLocalIndex(i);
        for( int z = 0; z < 4; z++)
        {
            int snapIndex = snapNodeMap[tetr.verts[z]];
            tetra->GetPointIds()->SetId( z, snapIndex );
        }
        grid->InsertNextCell(tetra->GetCellType(),tetra->GetPointIds());
    }
}
