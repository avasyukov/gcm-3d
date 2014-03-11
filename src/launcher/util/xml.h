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

		std::string getTagName();
		NodeList getChildNodes();
		NodeList getChildrenByName(std::string name);
                Node getChildByName(std::string name);
		AttrList getAttributes();
		NodeList xpath(std::string expr);
                
                std::string getTextContent();

	};

	class Doc {
	private:
		xmlDocPtr doc;
                
	public:
                Doc(xmlDocPtr doc);
		static Doc fromFile(std::string fname);
                static Doc fromString(std:: string str);
		~Doc();

		Node getRootElement();
	};
	std::string getAttributeByName(AttrList attrs, std::string name, std::string defaultValue);
	std::string getAttributeByName(AttrList attrs, std::string name);
};

#endif