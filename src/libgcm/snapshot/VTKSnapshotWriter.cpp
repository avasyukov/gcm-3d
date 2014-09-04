#include "libgcm/snapshot/VTKSnapshotWriter.hpp"

gcm::VTKSnapshotWriter::VTKSnapshotWriter() {
    INIT_LOGGER("gcm.VTKSnapshotWriter");
    fname = string ("snap_mesh_%m_cpu_%z_step_%n.vtu");
}

gcm::VTKSnapshotWriter::~VTKSnapshotWriter() {
}

gcm::VTKSnapshotWriter::VTKSnapshotWriter(const char* snapName) {
    INIT_LOGGER("gcm.VTKSnapshotWriter");
    fname = string (snapName);
}

string gcm::VTKSnapshotWriter::getType() {
    return "VTKSnapshotWriter";
}

void gcm::VTKSnapshotWriter::dump(Mesh* mesh, int step)
{
    // TODO - check if the mesh is compatible
    dumpVTK(getFileName(MPI::COMM_WORLD.Get_rank(), step, mesh->getId()), (TetrMeshSecondOrder*)mesh, step);
}

void gcm::VTKSnapshotWriter::dumpVTK(string filename, TetrMeshSecondOrder *mesh, int step)
{
    map<int, int> snapNodeMap;

    vtkXMLUnstructuredGridWriter *xgw = vtkXMLUnstructuredGridWriter::New();
    vtkUnstructuredGrid *g = vtkUnstructuredGrid::New();

    vtkPoints *pts = vtkPoints::New();

    vtkDoubleArray *vel = vtkDoubleArray::New();
    vel->SetNumberOfComponents(3);
    vtkDoubleArray *crack = vtkDoubleArray::New();
    crack->SetNumberOfComponents(3);
    vtkDoubleArray *force = vtkDoubleArray::New();
    force->SetNumberOfComponents(3);
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
    vtkIntArray    *borderId = vtkIntArray::New();
    vtkIntArray    *contactId = vtkIntArray::New();
    vtkDoubleArray *rho = vtkDoubleArray::New();
    vtkIntArray    *borderState = vtkIntArray::New();
    vtkIntArray    *mpiState = vtkIntArray::New();
    vtkIntArray    *nodeErrorFlags = vtkIntArray::New ();
    vtkIntArray    *contactDestroyed = vtkIntArray::New();
    vtkIntArray    *nodeDestroyed = vtkIntArray::New();
    vtkDoubleArray *damageMeasure = vtkDoubleArray::New();

    float v[3];
    int snapNodeCount = 0;
    float f[3];

    for(int i = 0; i < mesh->getNodesNumber(); i++)
    {
        CalcNode& node = mesh->getNodeByLocalIndex(i);

        if( node.isFirstOrder() )
        {
            snapNodeMap[ node.number ] = snapNodeCount;
            snapNodeCount++;
            pts->InsertNextPoint( node.coords[0], node.coords[1], node.coords[2] );
            v[0] = node.values[0];    v[1] = node.values[1];    v[2] = node.values[2];
            vel->InsertNextTuple(v);
            crack->InsertNextTuple(node.getCrackDirection().coords);
            if( node.isBorder() )
            {
                float n[3];
                mesh->findBorderNodeNormal(node.number, &n[0], &n[1], &n[2], false);
                f[0] = node.sxx * n[0] + node.sxy * n[1] + node.sxz * n[2];
                f[1] = node.sxy * n[0] + node.syy * n[1] + node.syz * n[2];
                f[2] = node.sxz * n[0] + node.syz * n[1] + node.szz * n[2];
            }
            else
            {
                f[0] = 0.0;
                f[1] = 0.0;
                f[2] = 0.0;
            }
            force->InsertNextTuple(f);
            sxx->InsertNextValue( node.values[3] );
            sxy->InsertNextValue( node.values[4] );
            sxz->InsertNextValue( node.values[5] );
            syy->InsertNextValue( node.values[6] );
            syz->InsertNextValue( node.values[7] );
            szz->InsertNextValue( node.values[8] );
            compression->InsertNextValue( node.getCompression() );
            tension->InsertNextValue( node.getTension() );
            shear->InsertNextValue( node.getShear() );
            deviator->InsertNextValue( node.getDeviator() );
            matId->InsertNextValue( node.getMaterialId() );
            borderId->InsertNextValue( node.getBorderConditionId() );
            contactId->InsertNextValue( node.getContactConditionId() );
            rho->InsertNextValue( node.getRho() );
            borderState->InsertNextValue( node.isBorder() ? ( node.isInContact() ? 2 : 1 ) : 0 );
            mpiState->InsertNextValue( node.isRemote() ? 1 : 0 );
            nodeErrorFlags->InsertNextValue (node.getErrorFlags());
            contactDestroyed->InsertNextValue( node.isContactDestroyed() ? 1 : 0 );
            nodeDestroyed->InsertNextValue( node.isDestroyed() ? 1 : 0 );
            damageMeasure->InsertNextValue( node.getDamageMeasure() );
        }
    }
    g->SetPoints(pts);

    vtkTetra *tetra=vtkTetra::New();
    for(int i = 0; i < mesh->getTetrsNumber(); i++)
    {
            TetrSecondOrder& tetr = mesh->getTetr2ByLocalIndex(i);
        for( int z = 0; z < 4; z++)
        {
            int snapIndex = snapNodeMap[ tetr.verts[z] ];
            tetra->GetPointIds()->SetId( z, snapIndex );
        }
        g->InsertNextCell(tetra->GetCellType(),tetra->GetPointIds());
    }

    vel->SetName("velocity");
    crack->SetName("crack");
    force->SetName("externalForce");
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
    borderId->SetName("borderConditionID");
    contactId->SetName("contactConditionID");
    rho->SetName("rho");
    borderState->SetName("borderState");
    mpiState->SetName("mpiState");
    nodeErrorFlags->SetName ("errorFlags");
    contactDestroyed->SetName("destroyedContacts");
    nodeDestroyed->SetName("destroyedNodes");
    damageMeasure->SetName("damageMeasure");

    g->GetPointData()->SetVectors(vel);
    g->GetPointData()->AddArray(crack);
    g->GetPointData()->AddArray(force);
    g->GetPointData()->AddArray(sxx);
    g->GetPointData()->AddArray(sxy);
    g->GetPointData()->AddArray(sxz);
    g->GetPointData()->AddArray(syy);
    g->GetPointData()->AddArray(syz);
    g->GetPointData()->AddArray(szz);
    g->GetPointData()->AddArray(compression);
    g->GetPointData()->AddArray(tension);
    g->GetPointData()->AddArray(shear);
    g->GetPointData()->AddArray(deviator);
    g->GetPointData()->AddArray(matId);
    g->GetPointData()->AddArray(borderId);
    g->GetPointData()->AddArray(contactId);
    g->GetPointData()->AddArray(rho);
    g->GetPointData()->AddArray(borderState);
    g->GetPointData()->AddArray(mpiState);
    g->GetPointData()->AddArray(nodeErrorFlags);
    g->GetPointData()->AddArray(contactDestroyed);
    g->GetPointData()->AddArray(nodeDestroyed);
    g->GetPointData()->AddArray(damageMeasure);

    vel->Delete();
    crack->Delete();
    force->Delete();
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
    borderId->Delete();
    contactId->Delete();
    rho->Delete();
    borderState->Delete();
    mpiState->Delete();
    nodeErrorFlags->Delete();
    contactDestroyed->Delete();
    nodeDestroyed->Delete();
    damageMeasure->Delete();
    
    #ifdef CONFIG_VTK_5
    xgw->SetInput(g);
    #else
    xgw->SetInputData(g);
    #endif
    xgw->SetFileName(filename.c_str());
    xgw->Update();

    xgw->Delete();
    g->Delete();
    pts->Delete();
    tetra->Delete();
}
