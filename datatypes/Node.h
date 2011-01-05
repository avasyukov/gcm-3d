#include <string>
#include <vector>

using std::string;
using std::vector;

class Node
{
public:
	Node();
	~Node();
	int zone_num;
	int local_num;
	int remote_num;
	int absolute_num;
	float coords[3];
	float fixed_coords[3];
protected:
private:
};

Node::Node() { }
Node::~Node() { }
