#ifndef GCM_VTK2SNAPSHOTWRITER_H_
#define GCM_VTK2SNAPSHOTWRITER_H_

#include <string>

#include "libgcm/util/Singleton.hpp"
#include "libgcm/snapshot/AbstractVTKSnapshotWriter.hpp"

namespace gcm {
    class TetrMeshSecondOrder;

    template <>
    MeshNodeIterator<TetrMeshSecondOrder, SNAPSHOTTER_ID_VTK2>& MeshNodeIterator<TetrMeshSecondOrder, SNAPSHOTTER_ID_VTK2>::operator++();

    class VTK2SnapshotWriter : public UnstructuredVTKSnapshotWriter<TetrMeshSecondOrder, false, SNAPSHOTTER_ID_VTK2>, public Singleton<VTK2SnapshotWriter> {
     protected:
        USE_LOGGER;
        void dumpMeshSpecificData(TetrMeshSecondOrder* mesh, vtkSmartPointer<vtkUnstructuredGrid>& grid, vtkSmartPointer<vtkPoints>& points) const override;
     public:
        VTK2SnapshotWriter();
    };
}


#endif
