#ifndef ABSTRACTVTKSNAPSHOTWRITER_HPP_
#define ABSTRACTVTKSNAPSHOTWRITER_HPP_


#include "libgcm/util/Assertion.hpp"
#include "libgcm/node/CalcNode.hpp"
#include "libgcm/Logging.hpp"

#ifdef CONFIG_VTK_5
#include <vtkstd/string>
#else
#include <vtkStdString.h>
#endif
#include <vtkStructuredGrid.h>
#include <vtkUnstructuredGrid.h>
#include <vtkXMLStructuredGridWriter.h>
#include <vtkXMLUnstructuredGridWriter.h>
#include <vtkSmartPointer.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>
#include <vtkCellData.h>

#define SNAPSHOTTER_ID_VTK 0
#define SNAPSHOTTER_ID_VTK2 1

namespace gcm
{
    template<typename MeshType, int id=0>
    class MeshNodeIterator
    {
     protected:
        MeshType* mesh;
        uint index = 0;
        void* data = nullptr;
        USE_LOGGER;
     public:
        MeshNodeIterator(MeshType* mesh): mesh(mesh)
        {
        }

        ~MeshNodeIterator()
        {
        }

        bool hasNext()
        {
            return index < mesh->getNodesNumber();
        }

        MeshNodeIterator<MeshType, id>& operator++()
        {
            index++;
            return *this;
        }

        MeshNodeIterator<MeshType, id>& operator++(int)
        {
            return ++(*this);
        }

        CalcNode& operator*()
        {
            return mesh->getNodeByLocalIndex(index);
        }
    };


    template<typename MeshType, typename GridType, typename GridWriterType, bool useCells=false, int snapshotterId=0>
    class AbstractVTKSnapshotWriter: public SnapshotWriter
    {
     protected:
        virtual void dumpMeshSpecificData(MeshType* mesh, vtkSmartPointer<GridType>& grid, vtkSmartPointer<vtkPoints>& points) const = 0;
        USE_LOGGER;
     public:
        AbstractVTKSnapshotWriter()
        {
            INIT_LOGGER("gcm.snapshot.AbstractVTKSnapshotWriter");
        }
        std::string dump(Mesh* mesh, int step, std::string fileName) const
        {
            auto _mesh = dynamic_cast<MeshType*>(mesh);
            assert_true(_mesh);

            LOG_DEBUG("Writing snapshot for mesh \"" << _mesh->getId() << "\" at step " << step << " to file " << fileName);

            auto grid = vtkSmartPointer<GridType>::New();
            auto points = vtkSmartPointer<vtkPoints>::New();

            auto contact = vtkSmartPointer<vtkIntArray>::New();
            contact->SetName("contact");

            auto border = vtkSmartPointer<vtkIntArray>::New();
            border->SetName("border");

            auto used = vtkSmartPointer<vtkIntArray>::New();
            used->SetName("used");

            auto norm = vtkSmartPointer<vtkDoubleArray>::New();
            norm->SetName("norm");
            norm->SetNumberOfComponents(3);

            auto vel = vtkSmartPointer<vtkDoubleArray>::New();
            vel->SetName("velocity");
            vel->SetNumberOfComponents(3);

            auto crack = vtkSmartPointer<vtkDoubleArray>::New();
            crack->SetName("crack");
            crack->SetNumberOfComponents(3);

            auto sxx = vtkSmartPointer<vtkDoubleArray>::New();
            sxx->SetName("sxx");

            auto sxy = vtkSmartPointer<vtkDoubleArray>::New();
            sxy->SetName("sxy");

            auto sxz = vtkSmartPointer<vtkDoubleArray>::New();
            sxz->SetName("sxz");

            auto syy = vtkSmartPointer<vtkDoubleArray>::New();
            syy->SetName("syy");

            auto syz = vtkSmartPointer<vtkDoubleArray>::New();
            syz->SetName("syz");

            auto szz = vtkSmartPointer<vtkDoubleArray>::New();
            szz->SetName("szz");

            auto compression = vtkSmartPointer<vtkDoubleArray>::New();
            compression->SetName("compression");

            auto tension = vtkSmartPointer<vtkDoubleArray>::New();
            tension->SetName("tension");

            auto shear = vtkSmartPointer<vtkDoubleArray>::New();
            shear->SetName("shear");

            auto deviator = vtkSmartPointer<vtkDoubleArray>::New();
            deviator->SetName("deviator");

            auto matId = vtkSmartPointer<vtkIntArray>::New();
            matId->SetName("materialID");

            auto rho = vtkSmartPointer<vtkDoubleArray>::New();
            rho->SetName("rho");

            auto mpiState = vtkSmartPointer<vtkIntArray>::New();
            mpiState->SetName("mpiState");

            auto nodePublicFlags = vtkSmartPointer<vtkIntArray>::New ();
            nodePublicFlags->SetName ("publicFlags");

            auto nodePrivateFlags = vtkSmartPointer<vtkIntArray>::New ();
            nodePrivateFlags->SetName ("privateFlags");

            auto nodeErrorFlags = vtkSmartPointer<vtkIntArray>::New ();
            nodeErrorFlags->SetName ("errorFlags");

            auto nodeNumber = vtkSmartPointer<vtkIntArray>::New ();
            nodeNumber->SetName ("nodeNumber");

            auto nodeBorderConditionId = vtkSmartPointer<vtkIntArray>::New ();
            nodeBorderConditionId->SetName ("borderConditionId");

            auto nodeContactConditionId = vtkSmartPointer<vtkIntArray>::New();
            nodeContactConditionId->SetName("contactState");

            auto contactDestroyed = vtkSmartPointer<vtkIntArray>::New();
            contactDestroyed->SetName("failedContacts");

            auto nodeDestroyed = vtkSmartPointer<vtkIntArray>::New();
            nodeDestroyed->SetName("failedNodes");

            auto nodeFailureMeasure = vtkSmartPointer<vtkDoubleArray>::New();
            nodeFailureMeasure->SetName("failureMeasure");

            float _norm[3];

            dumpMeshSpecificData(_mesh, grid, points);

            for (auto it = MeshNodeIterator<MeshType, snapshotterId>(_mesh); it.hasNext(); it++)
            {
                auto& node = *it;

                border->InsertNextValue(node.isBorder() ? 1 : 0);
                used->InsertNextValue(node.isUsed() ? 1 : 0);
                contact->InsertNextValue(node.isInContact() ? 1 : 0);

                if (node.isUsed() && node.isBorder())
                    _mesh->findBorderNodeNormal(node, _norm, _norm+1, _norm+2, false);
                else
                    _norm[0] = _norm[1] = _norm[2] = 0.0;
                norm->InsertNextTuple(_norm);

                vel->InsertNextTuple(node.velocity);
                crack->InsertNextTuple(node.getCrackDirection().coords);
                sxx->InsertNextValue(node.sxx);
                sxy->InsertNextValue(node.sxy);
                sxz->InsertNextValue(node.sxz);
                syy->InsertNextValue(node.syy);
                syz->InsertNextValue(node.syz);
                szz->InsertNextValue(node.szz);
                compression->InsertNextValue(node.getCompression());
                tension->InsertNextValue(node.getTension());
                shear->InsertNextValue(node.getShear());
                deviator->InsertNextValue(node.getDeviator());
                matId->InsertNextValue(node.getMaterialId());
                rho->InsertNextValue(node.getRho());
                mpiState->InsertNextValue(node.isRemote() ? 1 : 0);
                nodePrivateFlags->InsertNextValue (node.getPrivateFlags());
                nodePublicFlags->InsertNextValue (node.getPublicFlags());
                nodeErrorFlags->InsertNextValue (node.getErrorFlags());
                nodeBorderConditionId->InsertNextValue (node.getBorderConditionId());
				nodeContactConditionId->InsertNextValue(node.getContactConditionId());
                nodeNumber->InsertNextValue(node.number);
                contactDestroyed->InsertNextValue(node.isContactDestroyed() ? 1 : 0);
                nodeDestroyed->InsertNextValue(node.isDestroyed() ? 1 : 0);
                nodeFailureMeasure->InsertNextValue(node.getDamageMeasure());
            }

           vtkFieldData* fd;

           if (useCells)
               fd = grid->GetCellData();
           else
               fd = grid->GetPointData();

           grid->SetPoints(points);

           fd->AddArray(contact);
           fd->AddArray(border);
           fd->AddArray(used);
           fd->AddArray(norm);
           fd->AddArray(crack);
           fd->AddArray(sxx);
           fd->AddArray(sxy);
           fd->AddArray(sxz);
           fd->AddArray(syy);
           fd->AddArray(syz);
           fd->AddArray(szz);
           fd->AddArray(compression);
           fd->AddArray(tension);
           fd->AddArray(shear);
           fd->AddArray(deviator);
           fd->AddArray(matId);
           fd->AddArray(rho);
           fd->AddArray(mpiState);
           fd->AddArray (nodePrivateFlags);
           fd->AddArray (nodePublicFlags);
           fd->AddArray (nodeErrorFlags);
           fd->AddArray (nodeBorderConditionId);
           fd->AddArray (nodeContactConditionId);
           fd->AddArray(vel);
           fd->AddArray(nodeNumber);
           fd->AddArray(contactDestroyed);
           fd->AddArray(nodeDestroyed);
           fd->AddArray(nodeFailureMeasure);

           // Write file
           auto writer = vtkSmartPointer<GridWriterType>::New();
           writer->SetFileName(fileName.c_str());
           #ifdef CONFIG_VTK_5
           writer->SetInput(grid);
           #else
           writer->SetInputData(grid);
           #endif
           writer->Write();

           return fileName;
        }
    };

    template<typename MeshType, bool useCells=false, int id=0>
    using StructuredVTKSnapshotWriter = AbstractVTKSnapshotWriter<MeshType, vtkStructuredGrid, vtkXMLStructuredGridWriter, useCells, id>;

    template<typename MeshType, bool useCells=false, int id=0>
    using UnstructuredVTKSnapshotWriter = AbstractVTKSnapshotWriter<MeshType, vtkUnstructuredGrid, vtkXMLUnstructuredGridWriter, useCells, id>;
}



#endif /* ABSTRACTVTKSNAPSHOTWRITER_HPP_ */
