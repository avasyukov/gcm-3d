#include "libgcm/snapshot/VTKEulerMeshSnapshotWriter.hpp"

#include "libgcm/Math.hpp"
#include "libgcm/node/CalcNode.hpp"
#include "libgcm/mesh/euler/EulerMesh.hpp"

using namespace gcm;

VTKEulerMeshSnapshotWriter::VTKEulerMeshSnapshotWriter()
{
    INIT_LOGGER("gcm.snapshot.VTKEulerMeshSnapshotWriter");
    extension = "vts";
}

void VTKEulerMeshSnapshotWriter::dumpMeshSpecificData(EulerMesh* mesh, vtkSmartPointer<vtkStructuredGrid>& grid, vtkSmartPointer<vtkPoints>& points) const
{
    auto nodeDims = mesh->getNodeDimensions();
    auto cellDims = mesh->getDimensions();

    auto cellStatus = vtkSmartPointer<vtkIntArray>::New();
    cellStatus->SetName("cellStatus");

    auto cellError = vtkSmartPointer<vtkIntArray>::New();
    cellError->SetName("cellError");
    for (uint k = 0; k < nodeDims.z; k++)
        for (uint j = 0; j < nodeDims.y; j++)
            for (uint i = 0; i < nodeDims.x; i++)
            {
                auto& node = mesh->getNodeByEulerMeshIndex(vector3u(i, j, k));
                points->InsertNextPoint(node.coords.x, node.coords.y, node.coords.z);
            }
    for (uint k = 0; k < cellDims.z; k++)
        for (uint j = 0; j < cellDims.y; j++)
            for (uint i = 0; i < cellDims.x; i++)
            {
                cellStatus->InsertNextValue(mesh->getCellStatus(vector3u(i, j, k)));
                char flag = 0;

                for (uint p = 0; p <= 1; p++)
                    for (uint q = 0; q <= 1; q++)
                        for (uint s = 0; s <= 1; s++)
                            if ((flag = mesh->getNodeByEulerMeshIndex(vector3u(i+p, j+q, k+s)).getErrorFlags()))
                                break;

                cellError->InsertNextValue(flag);
            }

    grid->GetCellData()->AddArray(cellStatus);
    grid->GetCellData()->AddArray(cellError);

    grid->SetDimensions(nodeDims.x, nodeDims.y, nodeDims.z);
}
