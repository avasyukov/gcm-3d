#include "libgcm/mesh/markers/MarkeredSurface.hpp"

#include <algorithm>



#ifdef CONFIG_VTK_5
#include <vtkstd/string>
#else
#include <vtkStdString.h>
#endif
#include <vtkUnstructuredGrid.h>
#include <vtkXMLUnstructuredGridWriter.h>
#include <vtkUnstructuredGridWriter.h>
#include <vtkTetra.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>
#include <vtkCellData.h>

using namespace gcm;
using namespace std;

MarkeredSurface::MarkeredSurface()
{
}
MarkeredSurface::MarkeredSurface(vector<CalcNode>& markers, vector<TriangleFirstOrder>& faces, vector<int>& regions)
{
    this->markers = move(markers);
    this->faces = move(faces);
    this->regions = move(regions);

    updateAABB();





/*
    vtkXMLUnstructuredGridWriter *xgw = vtkXMLUnstructuredGridWriter::New();
    vtkUnstructuredGrid *g = vtkUnstructuredGrid::New();

    vtkPoints *pts = vtkPoints::New();


    for(auto m: this->markers)
    {
        pts->InsertNextPoint( m.coords[0], m.coords[1], m.coords[2] );
    }
    pts->InsertNextPoint( 0, 0, 0);
    g->SetPoints(pts);

    vtkTetra* tetra = vtkTetra::New();

    for(auto f: this->faces)
    {
        for( int z = 0; z < 3; z++)
        {
            tetra->GetPointIds()->SetId( z, f.verts[z]);
        }
           tetra->GetPointIds()->SetId( 3, this->markers.size());
        g->InsertNextCell(tetra->GetCellType(),tetra->GetPointIds());
    }

    
    #ifdef CONFIG_VTK_5
    xgw->SetInput(g);
    #else
    xgw->SetInputData(g);
    #endif
    xgw->SetFileName("/tmp/1.vtu");
    xgw->Update();

    xgw->Delete();
    g->Delete();
    pts->Delete();
    tetra->Delete();


*/
}

const vector<CalcNode>& MarkeredSurface::getMarkerNodes() const
{
    return markers;
}

unsigned int MarkeredSurface::getNumberOfMarkerNodes() const
{
    return markers.size();
}

const vector<TriangleFirstOrder>& MarkeredSurface::getMarkerFaces() const
{
    return faces;
}

unsigned int MarkeredSurface::getNumberOfMarkerFaces() const
{
    return faces.size();
}

const AABB& MarkeredSurface::getAABB() const
{
    return aabb;
}

void MarkeredSurface::updateAABB()
{
    AABB aabb;

    for (auto m: markers)
        for (int i = 0; i < 3; i++)
        {
            if (m.coords[i] < aabb.min_coords[i])
                aabb.min_coords[i] = m.coords[i];
            if (m.coords[i] > aabb.max_coords[i])
                aabb.max_coords[i] = m.coords[i];
        }

    this->aabb = aabb;
}

const std::vector<  int> MarkeredSurface::getRegions() const
{
    return regions;
}
