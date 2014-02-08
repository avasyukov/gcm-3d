#include <map>
#include <string>
#include <vector>

#include <libxml/parser.h>
#include <libxml/xpath.h>


namespace xml {

	class Node;

	typedef std::vector<Node> NodeList;
	typedef std::map<std::string, std::string> AttrList;

	class Node {
	private:
		xmlNodePtr node;
	public:
		Node(xmlNodePtr node);

		std::string getTagName();
		NodeList getChildNodes();
		NodeList getChildrenByName(std::string name);
		AttrList getAttributes();
		NodeList xpath(std::string expr);

	};

	class Doc {
	private:
		xmlDocPtr doc;
	public:
		Doc(std::string fname);
		~Doc();

		Node getRootElement();
	};
	std::string getAttributeByName(AttrList attrs, std::string name, std::string defaultValue);
	std::string getAttributeByName(AttrList attrs, std::string name);
};
