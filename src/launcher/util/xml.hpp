#ifndef XML_H
#define XML_H

#include <map>
#include <string>
#include <vector>

#include <libxml/parser.h>
#include <libxml/xpath.h>

#include "libgcm/Logging.hpp"

namespace xml {

    class Node;

    typedef std::vector<Node> NodeList;
    typedef std::map<std::string, std::string> AttrList;

    class Node {
    private:
        xmlNodePtr node;
        xmlXPathContextPtr ctx = nullptr;
        USE_LOGGER;
    public:
        Node(const xmlNodePtr node, const xmlXPathContextPtr ctx);

        std::string getTagName() const;
        NodeList getChildNodes() const;
        NodeList getChildrenByName(const std::string& name) const;
        Node getChildByName(const std::string& name) const;
        AttrList getAttributes() const;
        NodeList xpath(const std::string& expr) const;
        std::string getTextContent() const;

        std::string operator[](const std::string& name) const;
        std::string getAttributeByName(const std::string& name) const;
        std::string getAttributeByName(const std::string& name, const std::string& defaultValue) const;
        bool hasAttribute(const std::string& name) const;

    };

    class Doc {
    private:
        xmlDocPtr doc;
        xmlXPathContextPtr ctx = nullptr;

    public:
        Doc(const xmlDocPtr doc);
        Doc(Doc&& d);

        void registerNamespace(const std::string& prefix, const std::string& uri);

        static Doc fromFile(const std::string& fname);
        static Doc fromString(const std::string& str);
        ~Doc();

        void operator=(Doc&& d);

        Node getRootElement() const;
    };
};

#endif
