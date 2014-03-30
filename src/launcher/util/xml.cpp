#include "launcher/util/xml.h"

#include <string.h>

#include "libgcm/Exception.h"

xml::Doc::Doc(xmlDocPtr doc): doc(doc)
{

}

xml::Doc xml::Doc::fromFile(std::string fname)
{
    return Doc(xmlParseFile(fname.c_str()));
}

xml::Doc xml::Doc::fromString(std::string str)
{
    return Doc(xmlParseDoc((xmlChar*)str.c_str()));
}

xml::Doc::~Doc()
{
// FIXME
// this causes segfaults when using Doc::fromString more than once
//    xmlFreeDoc(doc);
}

xml::Node xml::Doc::getRootElement() const
{
    return  Node(xmlDocGetRootElement(doc));
}

xml::Node::Node(xmlNodePtr node)
{
    INIT_LOGGER("xml.Node");
    this->node = node;
}

std::string xml::Node::getTagName() const
{
    std::string name((char*) node->name);
    return name;
}

xml::NodeList xml::Node::getChildNodes() const
{
    NodeList nodes;
    for (xmlNodePtr child = node->children; child; child = child->next)
        if (child->type == XML_ELEMENT_NODE) {
            Node node(child);
            nodes.push_back(node);
        }
    return nodes;
}

xml::NodeList xml::Node::getChildrenByName(std::string name) const
{
    NodeList nodes;
    for (xmlNodePtr child = node->children; child; child = child->next)
        if (child->type == XML_ELEMENT_NODE && !strcmp((char*) child->name, name.c_str())) {
            Node node(child);
            nodes.push_back(node);
        }
    return nodes;
}

xml::Node xml::Node::getChildByName(std::string name) const
{
    NodeList nodes = getChildrenByName(name);
    auto l = nodes.size();
    if (l != 1) {
        LOG_ERROR("Expected one node with name " << name << " but found " << l);
        THROW_INVALID_ARG("Child not found");
    }
    return nodes[0];
}

xml::AttrList xml::Node::getAttributes() const
{
    xml::AttrList attrs;

    for (xmlAttrPtr attr = node->properties; attr; attr = attr->next) {
        std::string name((char*) attr->name);
        std::string value((char*) attr->children->content);
        attrs[name] = value;
    }

    return attrs;
}

xml::NodeList xml::Node::xpath(std::string expr) const
{
    xml::NodeList nodes;

    xmlXPathContextPtr ctx = xmlXPathNewContext(node->doc);
    ctx->node = node;
    xmlXPathObjectPtr obj = xmlXPathEvalExpression((xmlChar*) expr.c_str(), ctx);

    if (!xmlXPathNodeSetIsEmpty(obj->nodesetval))
        for (int i = 0; i < obj->nodesetval->nodeNr; i++) {
            xmlNodePtr node = obj->nodesetval->nodeTab[i];
            if (node->type == XML_ELEMENT_NODE) {
                Node _node(node);
                nodes.push_back(_node);
            }
        }

    xmlXPathFreeObject(obj);
    xmlXPathFreeContext(ctx);

    return nodes;
}

std::string xml::Node::getTextContent() const
{
    return std::string((char*)xmlNodeGetContent(node));
}

/*
 * Returns value of named attribute.
 */

std::string xml::getAttributeByName(AttrList attrs, std::string name, std::string defaultValue)
{
    AttrList::iterator iter = attrs.find(name);
    if (iter != attrs.end())
        return iter->second;
    if (defaultValue != "")
        return defaultValue;
    THROW_INVALID_ARG("Attribute \"" + name + "\" not found in list and default value is not provided");
}

std::string xml::getAttributeByName(Node node, std::string name, std::string defaultValue)
{
    return getAttributeByName(node.getAttributes(), name, defaultValue);
}

std::string xml::getAttributeByName(AttrList attrs, std::string name)
{
    return getAttributeByName(attrs, name, "");
}

std::string xml::getAttributeByName(Node node, std::string name)
{
    return getAttributeByName(node.getAttributes(), name);
}
