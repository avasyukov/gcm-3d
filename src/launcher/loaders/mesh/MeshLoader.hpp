#ifndef LAUNCHER_MESHLOADER_H
#define LAUNCHER_MESHLOADER_H

#include "launcher/util/xml.hpp"

#include "libgcm/Body.hpp"

using namespace gcm;

namespace launcher
{
    template<typename MeshType>
    class MeshLoader
    {
        protected:
            virtual void loadMesh(const xml::Node& desc, MeshType* mesh) = 0;
        public:
            MeshType* load(const xml::Node& desc, Body* body)
            {
                MeshType* mesh = new MeshType();
                mesh->setId(desc["id"]);
                mesh->setCalc(desc.getAttributeByName("calc", "false") == "true");
                mesh->setBody(body);
                loadMesh(desc, mesh);
                return mesh;
            }
    };
}

#endif

