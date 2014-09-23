#include "libgcm/snapshot/VTKMarkeredMeshSnapshotWriter.hpp"

#include "libgcm/Math.hpp"
#include "libgcm/node/CalcNode.hpp"
#include "libgcm/mesh/markers/MarkeredMesh.hpp"

#ifdef CONFIG_VTK_5
#include <vtkstd/string>
#else
#include <vtkStdString.h>
#endif
#include <vtkStructuredGrid.h>
#include <vtkXMLStructuredGridWriter.h>
#include <vtkSmartPointer.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>
#include <vtkCellData.h>

using std::string;

gcm::VTKMarkeredMeshSnapshotWriter::VTKMarkeredMeshSnapshotWriter()
{
    INIT_LOGGER("gcm.snapshot.VTKMarkeredMeshSnapshotWriter");
    extension = "vts";
}

string gcm::VTKMarkeredMeshSnapshotWriter::dump(Mesh* mesh, int step, std::string fileName) const
{
    return dumpVTK(fileName, *dynamic_cast<const MarkeredMesh*>(mesh), step);
}

string gcm::VTKMarkeredMeshSnapshotWriter::dumpVTK(string filename, const MarkeredMesh& mesh, int step) const
{
    LOG_DEBUG("Writing snapshot for mesh \"" << mesh.getId() << "\" at step " << step << " to file " << filename);
    
    vtkSmartPointer<vtkStructuredGrid> structuredGrid = vtkSmartPointer<vtkStructuredGrid>::New();
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();

    vtkIntArray* border = vtkIntArray::New();
    vtkIntArray* used = vtkIntArray::New();
    vtkDoubleArray* norm = vtkDoubleArray::New();
    norm->SetNumberOfComponents(3);
    vtkDoubleArray *vel = vtkDoubleArray::New();
    vel->SetNumberOfComponents(3);
    vtkDoubleArray *crack = vtkDoubleArray::New();
    crack->SetNumberOfComponents(3);
    vtkDoubleArray *sxx = vtkDoubleArray::New();
    vtkDoubleArray *sxy = vtkDoubleArray::New();
    vtkDoubleArray *sxz = vtkDoubleArray::New();
    vtkDoubleArray *syy = vtkDoubleArray::New();
    vtkDoubleArray *syz = vtkDoubleArray::New();
    vtkDoubleArray *szz = vtkDoubleArray::New();
    vtkDoubleArray *compression = vtkDoubleArray::New();
    vtkDoubleArray *tension = vtkDoubleArray::New();
    vtkDoubleArray *shear = vtkDoubleArray::New();
    vtkDoubleArray *deviator = vtkDoubleArray::New();
    vtkIntArray    *matId = vtkIntArray::New();
    vtkDoubleArray *rho = vtkDoubleArray::New();
    vtkIntArray    *borderState = vtkIntArray::New();
    vtkIntArray    *contactState = vtkIntArray::New();
    vtkIntArray    *mpiState = vtkIntArray::New();
    vtkIntArray    *nodeErrorFlags = vtkIntArray::New ();
    

    float _norm[3];

    auto meshElems = mesh.getMeshElemes();
    auto elemSize = mesh.getElemSize();
    
    vector3r coords  = mesh.getPivot();
    
    for (unsigned int i = 0; i <= meshElems; i++)
        for (unsigned int j = 0; j <= meshElems; j++)
            for (unsigned int k = 0; k <= meshElems; k++)
                points->InsertNextPoint(coords.x+i*elemSize, coords.y+j*elemSize, coords.z+k*elemSize);
    
    auto& _mesh = const_cast<MarkeredMesh&>(mesh);

    for (unsigned int i = 0; i < meshElems; i++)
        for (unsigned int j = 0; j < meshElems; j++)
            for (unsigned int k = 0; k < meshElems; k++)
            {
                auto& cell = _mesh.getCellByLocalIndex(i, j, k);
                border->InsertNextValue(cell.isBorder() ? 1 : 0);
                used->InsertNextValue(cell.isUsed() ? 1 : 0);
                if (cell.isBorder())
                    _mesh.findBorderNodeNormal(cell.number, _norm, _norm+1, _norm+2, false);
                else
                    _norm[0] = _norm[1] = _norm[2] = 0.0;
                norm->InsertNextTuple(_norm);

                vel->InsertNextTuple(cell.velocity);
                crack->InsertNextTuple(cell.getCrackDirection().coords);
                sxx->InsertNextValue(cell.sxx);
                sxy->InsertNextValue(cell.sxy);
                sxz->InsertNextValue(cell.sxz);
                syy->InsertNextValue(cell.syy);
                syz->InsertNextValue(cell.syz);
                szz->InsertNextValue(cell.szz);
                compression->InsertNextValue(cell.getCompression());
                tension->InsertNextValue(cell.getTension());
                shear->InsertNextValue(cell.getShear());
                deviator->InsertNextValue(cell.getDeviator());
                matId->InsertNextValue(cell.getMaterialId());
                rho->InsertNextValue(cell.getRho());
                borderState->InsertNextValue(cell.isBorder() ? ( cell.isInContact() ? 2 : 1 ) : 0);
                contactState->InsertNextValue(cell.getContactConditionId());
                mpiState->InsertNextValue(cell.isRemote() ? 1 : 0);
                nodeErrorFlags->InsertNextValue (cell.getErrorFlags());
            }

    structuredGrid->SetDimensions(meshElems+1, meshElems+1, meshElems+1);
    structuredGrid->SetPoints(points);

    border->SetName("border");
    used->SetName("used");
    norm->SetName("norm");
    vel->SetName("velocity");
    crack->SetName("crack");
    sxx->SetName("sxx");
    sxy->SetName("sxy");
    sxz->SetName("sxz");
    syy->SetName("syy");
    syz->SetName("syz");
    szz->SetName("szz");
    compression->SetName("compression");
    tension->SetName("tension");
    shear->SetName("shear");
    deviator->SetName("deviator");
    matId->SetName("materialID");
    rho->SetName("rho");
    borderState->SetName("borderState");
    contactState->SetName("contactState");
    mpiState->SetName("mpiState");
    nodeErrorFlags->SetName ("errorFlags");
    
    

    structuredGrid->GetCellData()->AddArray(border);
    structuredGrid->GetCellData()->AddArray(used);
    structuredGrid->GetCellData()->AddArray(norm);
    structuredGrid->GetCellData()->AddArray(crack);
    structuredGrid->GetCellData()->AddArray(sxx);
    structuredGrid->GetCellData()->AddArray(sxy);
    structuredGrid->GetCellData()->AddArray(sxz);
    structuredGrid->GetCellData()->AddArray(syy);
    structuredGrid->GetCellData()->AddArray(syz);
    structuredGrid->GetCellData()->AddArray(szz);
    structuredGrid->GetCellData()->AddArray(compression);
    structuredGrid->GetCellData()->AddArray(tension);
    structuredGrid->GetCellData()->AddArray(shear);
    structuredGrid->GetCellData()->AddArray(deviator);
    structuredGrid->GetCellData()->AddArray(matId);
    structuredGrid->GetCellData()->AddArray(rho);
    structuredGrid->GetCellData()->AddArray(borderState);
    structuredGrid->GetCellData()->AddArray(contactState);
    structuredGrid->GetCellData()->AddArray(mpiState);
    structuredGrid->GetCellData()->AddArray (nodeErrorFlags);
    structuredGrid->GetCellData()->AddArray(vel);


    // Write file
    vtkSmartPointer<vtkXMLStructuredGridWriter> writer = vtkSmartPointer<vtkXMLStructuredGridWriter>::New();
    writer->SetFileName(filename.c_str());
    #ifdef CONFIG_VTK_5
    writer->SetInput(structuredGrid);
    #else
    writer->SetInputData(structuredGrid);
    #endif
    writer->Write();
    
    border->Delete();
    used->Delete();
    vel->Delete();
    crack->Delete();
    sxx->Delete();
    sxy->Delete();
    sxz->Delete();
    syy->Delete();
    syz->Delete();
    szz->Delete();
    compression->Delete();
    tension->Delete();
    shear->Delete();
    deviator->Delete();
    matId->Delete();
    rho->Delete();
    borderState->Delete();
    contactState->Delete();
    mpiState->Delete();
    nodeErrorFlags->Delete();

    return filename;
}
