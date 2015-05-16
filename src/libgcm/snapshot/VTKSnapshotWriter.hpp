#ifndef GCM_VTKSNAPSHOTWRITER_H_
#define GCM_VTKSNAPSHOTWRITER_H_

#include <string>

#include "libgcm/util/Singleton.hpp"
#include "libgcm/snapshot/AbstractVTKSnapshotWriter.hpp"


namespace gcm {
    class TetrMeshFirstOrder;

    template <>
    MeshNodeIterator<TetrMeshFirstOrder, SNAPSHOTTER_ID_VTK>& MeshNodeIterator<TetrMeshFirstOrder, SNAPSHOTTER_ID_VTK>::operator++();

    class VTKSnapshotWriter : public UnstructuredVTKSnapshotWriter<TetrMeshFirstOrder, false, SNAPSHOTTER_ID_VTK>, public Singleton<VTKSnapshotWriter> {
     protected:
        USE_LOGGER;
        void dumpMeshSpecificData(TetrMeshFirstOrder* mesh, vtkSmartPointer<vtkUnstructuredGrid>& grid, vtkSmartPointer<vtkPoints>& points) const override;
     public:
        VTKSnapshotWriter();
    };
}


#endif
