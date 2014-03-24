#ifndef XML_H
#define XML_H

#include <map>
#include <string>
#include <vector>

#include <libxml/parser.h>
#include <libxml/xpath.h>

#include "Logging.h"


namespace xml {

    class Node;

    typedef std::vector<Node> NodeList;
    typedef std::map<std::string, std::string> AttrList;

    class Node {
    private:
        xmlNodePtr node;
                USE_LOGGER;
    public:
        Node(xmlNodePtr node);

        std::string getTagName() const;
        NodeList getChildNodes() const;
        NodeList getChildrenByName(std::string name) const;
        Node getChildByName(std::string name) const;
        AttrList getAttributes() const;
        NodeList xpath(std::string expr) const;
        std::string getTextContent() const;

    };

    class Doc {
    private:
        xmlDocPtr doc;

    public:
        Doc(xmlDocPtr doc);
        static Doc fromFile(std::string fname);
        static Doc fromString(std:: string str);
        ~Doc();

        Node getRootElement() const;
    };
    std::string getAttributeByName(AttrList attrs, std::string name, std::string defaultValue);
    std::string getAttributeByName(AttrList attrs, std::string name);
};

#endif
