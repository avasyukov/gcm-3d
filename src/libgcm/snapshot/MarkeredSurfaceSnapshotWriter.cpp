#include "libgcm/snapshot/MarkeredSurfaceSnapshotWriter.hpp"

#include "libgcm/util/Assertion.hpp"
#include "libgcm/mesh/euler/markers/MarkeredMesh.hpp"

#ifdef CONFIG_VTK_5
#include <vtkstd/string>
#else
#include <vtkStdString.h>
#endif
#include <vtkStructuredGrid.h>
#include <vtkUnstructuredGrid.h>
#include <vtkXMLUnstructuredGridWriter.h>
#include <vtkSmartPointer.h>
#include <vtkTetra.h>
#include <vtkPoints.h>

using std::string;
using namespace gcm;

MarkeredSurfaceSnapshotWriter::MarkeredSurfaceSnapshotWriter() {
    suffix = "-surface";
    extension = "vtu";
    INIT_LOGGER("gcm.snapshot.MarkeredSurfaceSnapshotWriter");
}

string MarkeredSurfaceSnapshotWriter::dump(Mesh* mesh, int step, string fileName) const {

    auto _mesh = dynamic_cast<MarkeredMesh*>(mesh);
    assert_true(_mesh);

    LOG_DEBUG("Writing snapshot for mesh \"" << _mesh->getId() << "\" at step " << step << " to file " << fileName);

    auto grid = vtkSmartPointer<vtkUnstructuredGrid>::New();
    auto points = vtkSmartPointer<vtkPoints>::New();


    for(auto m: _mesh->getSurface().getMarkerNodes())
        points->InsertNextPoint(m.coords[0], m.coords[1], m.coords[2]);
    auto center = _mesh->getSurface().getAABB().getCenter();
    points->InsertNextPoint(center.x, center.y, center.z);

    grid->SetPoints(points);

    auto tetra = vtkSmartPointer<vtkTetra>::New();
    auto lastPoint = _mesh->getSurface().getNumberOfMarkerNodes();

    for(auto f: _mesh->getSurface().getMarkerFaces())
    {
        for( int z = 0; z < 3; z++)
            tetra->GetPointIds()->SetId( z, f.verts[z]);
        tetra->GetPointIds()->SetId(3, lastPoint);
        grid->InsertNextCell(tetra->GetCellType(),tetra->GetPointIds());
    }

    auto writer = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
    writer->SetFileName(fileName.c_str());
    #ifdef CONFIG_VTK_5
    writer->SetInput(grid);
    #else
    writer->SetInputData(grid);
    #endif
    writer->Write();

    return fileName;
}
