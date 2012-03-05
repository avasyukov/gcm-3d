#ifndef _GCM3D
#define _GCM3D  1

#include "datatypes/Basis.h"
#include "datatypes/ContactData.h"
#include "datatypes/MeshOutline.h"

#include "datatypes/Node.h"
#include "datatypes/ElasticNode.h"

#include "datatypes/Element.h"
#include "datatypes/Triangle.h"
#include "datatypes/Tetrahedron.h"
#include "datatypes/Tetrahedron_1st_order.h"

#include "datatypes/matrixes.h"
#include "datatypes/ElasticMatrix3D.h"

#include "rheotypes/RheologyCalculator.h"
#include "rheotypes/VoidRheologyCalculator.h"

#include "meshtypes/Mesh.h"
#include "meshtypes/TetrMesh.h"
#include "meshtypes/TetrMesh_1stOrder.h"

#include "methods/NumericalMethod.h"
#include "methods/TetrNumericalMethod.h"
#include "methods/TetrMethod_Plastic_1stOrder.h"

#include "system/quick_math.h"
#include "system/DataBus.h"
#include "system/TetrMeshSet.h"
#include "system/GCMStress.h"
#include "system/Stresser.h"
#include "system/TaskPreparator.h"
#include "system/CylTest.h"
#include "system/CollisionDetector.h"
#include "system/SnapshotWriter.h"
#include "system/Logger.h"

#endif
