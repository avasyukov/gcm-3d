#ifndef GCM_VTKSNAPSHOTWRITER_H_
#define GCM_VTKSNAPSHOTWRITER_H_

#include <string>

#include "libgcm/util/Singleton.hpp"
#include "libgcm/snapshot/AbstractVTKSnapshotWriter.hpp"


namespace gcm {
    class TetrMeshSecondOrder;

    template <>
    MeshNodeIterator<TetrMeshSecondOrder, SNAPSHOTTER_ID_VTK>& MeshNodeIterator<TetrMeshSecondOrder, SNAPSHOTTER_ID_VTK>::operator++();

    class VTKSnapshotWriter : public UnstructuredVTKSnapshotWriter<TetrMeshSecondOrder, false, SNAPSHOTTER_ID_VTK>, public Singleton<VTKSnapshotWriter> {
     protected:
        USE_LOGGER;
        void dumpMeshSpecificData(TetrMeshSecondOrder* mesh, vtkSmartPointer<vtkUnstructuredGrid>& grid, vtkSmartPointer<vtkPoints>& points) const override;
     public:
        VTKSnapshotWriter();
    };
}


#endif
