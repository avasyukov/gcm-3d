#include "xml.h"

#include <string.h>

#include "Exception.h"

xml::Doc::Doc(std::string fname) {
	doc = xmlParseFile(fname.c_str());
}

xml::Doc::~Doc() {
	xmlFreeDoc(doc);
}

xml::Node xml::Doc::getRootElement() {
	Node node(xmlDocGetRootElement(doc));
	return node;
}

xml::Node::Node(xmlNodePtr node) {
	this->node = node;
}

std::string xml::Node::getTagName() {
	std::string name((char*)node->name);
	return name;
}

xml::NodeList xml::Node::getChildNodes() {
	NodeList nodes;
	for (xmlNodePtr child = node->children; child; child = child->next) 
		if (child->type == XML_ELEMENT_NODE){
			Node node(child);
			nodes.push_back(node);
		}
	return nodes;
}

xml::NodeList xml::Node::getChildrenByName(std::string name) {
	NodeList nodes;
	for (xmlNodePtr child = node->children; child; child = child->next) 
		if (child->type == XML_ELEMENT_NODE && !strcmp((char*)child->name, name.c_str())){
			Node node(child);
			nodes.push_back(node);
		}
	return nodes;
}

xml::AttrList xml::Node::getAttributes() {
	xml::AttrList attrs;
	
	for (xmlAttrPtr attr = node->properties; attr; attr = attr->next) {
		std::string name((char*)attr->name);
		std::string value((char*)attr->children->content);
		attrs[name] = value;
	}

	return attrs;
}

xml::NodeList xml::Node::xpath(std::string expr) {
	xml::NodeList nodes;

	xmlXPathContextPtr ctx = xmlXPathNewContext(node->doc);
	ctx->node = node;
	xmlXPathObjectPtr obj = xmlXPathEvalExpression((xmlChar*)expr.c_str(), ctx);

	if(!xmlXPathNodeSetIsEmpty(obj->nodesetval))
		for (int i = 0; i < obj->nodesetval->nodeNr; i++) {
			xmlNodePtr node = obj->nodesetval->nodeTab[i];
			if (node->type == XML_ELEMENT_NODE){
				Node _node(node);
				nodes.push_back(_node);
			}
		}

	xmlXPathFreeObject(obj);
	xmlXPathFreeContext(ctx);

	return nodes;
}

/*
 * Returns value of named attribute.
 */

std::string xml::getAttributeByName(AttrList attrs, std::string name, std::string defaultValue) {
	AttrList::iterator iter = attrs.find(name);
	if (iter != attrs.end())
		return iter->second;
	if( defaultValue != "" )
		return defaultValue;
	THROW_INVALID_ARG("Attribute \"" + name + "\" not found in list and default value is not provided");
}

std::string xml::getAttributeByName(AttrList attrs, std::string name) {
	return getAttributeByName(attrs, name, "");
}