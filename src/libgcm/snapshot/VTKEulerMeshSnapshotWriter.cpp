#include "libgcm/snapshot/VTKEulerMeshSnapshotWriter.hpp"

#include "libgcm/Math.hpp"
#include "libgcm/node/CalcNode.hpp"
#include "libgcm/mesh/euler/EulerMesh.hpp"

#include <unordered_map>

using namespace gcm;

using std::unordered_map;

namespace gcm
{
    typedef struct
    {
        vector3u index;
        vector3u dims;
    } data_t;

    template<>
    MeshNodeIterator<EulerMesh, 0>::MeshNodeIterator(EulerMesh* mesh): mesh(mesh)
    {
        INIT_LOGGER("gcm.snapshot.MeshNodeIterator.EulerMesh");
        data = new data_t;
        static_cast<data_t*>(data)->dims = mesh->getNodeDimensions();
    }

    template<>
    MeshNodeIterator<EulerMesh, 0>::~MeshNodeIterator()
    {
        delete static_cast<data_t*>(data);
    }

    template<>
    bool MeshNodeIterator<EulerMesh, 0>::hasNext()
    {
        return static_cast<data_t*>(data)->index << static_cast<data_t*>(data)->dims;
    }

    template<>
    MeshNodeIterator<EulerMesh, 0>& MeshNodeIterator<EulerMesh, 0>::operator++()
    {
        vector3u& v = static_cast<data_t*>(data)->index;
        const vector3u& dims = static_cast<data_t*>(data)->dims;

        v.x++;
        if (v.x == dims.x)
        {
            v.x = 0;
            v.y++;
            if (v.y == dims.y)
            {
                v.y = 0;
                v.z++;
            }
        }

        return *this;
    }

    template<>
    MeshNodeIterator<EulerMesh, 0>& MeshNodeIterator<EulerMesh, 0>::operator++(int)
    {
        return ++(*this);
    }

    template<>
    CalcNode& MeshNodeIterator<EulerMesh, 0>::operator*()
    {
        return mesh->getNodeByEulerMeshIndex(static_cast<data_t*>(data)->index);
    }
};

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

    auto materialId = vtkSmartPointer<vtkIntArray>::New();
    materialId->SetName("materialId");

    for (uint k = 0; k < nodeDims.z; k++)
        for (uint j = 0; j < nodeDims.y; j++)
            for (uint i = 0; i < nodeDims.x; i++)
            {
                auto& node = mesh->getNodeByEulerMeshIndex(vector3u(i, j, k));
                points->InsertNextPoint(node.coords.x, node.coords.y, node.coords.z);
            }

    unordered_map<int, int> materials;

    for (uint k = 0; k < cellDims.z; k++)
        for (uint j = 0; j < cellDims.y; j++)
            for (uint i = 0; i < cellDims.x; i++)
            {
                cellStatus->InsertNextValue(mesh->getCellStatus(vector3u(i, j, k)));
                char flag = 0;

                materials.clear();
                for (uint p = 0; p <= 1; p++)
                    for (uint q = 0; q <= 1; q++)
                        for (uint s = 0; s <= 1; s++)
                        {
                            const auto& node = mesh->getNodeByEulerMeshIndex(vector3u(i+p, j+q, k+s));
                            flag |= node.getErrorFlags();
                            materials[node.getMaterialId()]++;
                        }

                int mat = -1;
                int max = -1;
                for (auto& m: materials)
                    if (m.second > max)
                    {
                        max = m.second;
                        mat = m.first;
                    }

                cellError->InsertNextValue(flag);
                materialId->InsertNextValue(mat);
            }

    grid->GetCellData()->AddArray(cellStatus);
    grid->GetCellData()->AddArray(cellError);
    grid->GetCellData()->AddArray(materialId);

    grid->SetDimensions(nodeDims.x, nodeDims.y, nodeDims.z);
}
