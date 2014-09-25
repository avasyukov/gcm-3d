#include "libgcm/snapshot/VTKMarkeredMeshSnapshotWriter.hpp"

#include "libgcm/Math.hpp"
#include "libgcm/node/CalcNode.hpp"
#include "libgcm/mesh/markers/MarkeredMesh.hpp"

using namespace gcm;

VTKMarkeredMeshSnapshotWriter::VTKMarkeredMeshSnapshotWriter()
{
    INIT_LOGGER("gcm.snapshot.VTKMarkeredMeshSnapshotWriter");
    extension = "vts";
}

void VTKMarkeredMeshSnapshotWriter::dumpMeshSpecificData(MarkeredMesh* mesh, vtkSmartPointer<vtkStructuredGrid>& grid, vtkSmartPointer<vtkPoints>& points) const
{
    auto meshElems = mesh->getMeshElemes();
    auto elemSize = mesh->getElemSize();
    
    vector3r coords  = mesh->getPivot();
    
    for (unsigned int i = 0; i <= meshElems; i++)
        for (unsigned int j = 0; j <= meshElems; j++)
            for (unsigned int k = 0; k <= meshElems; k++)
                points->InsertNextPoint(coords.x+i*elemSize, coords.y+j*elemSize, coords.z+k*elemSize);
    
    grid->SetDimensions(meshElems+1, meshElems+1, meshElems+1);
}
