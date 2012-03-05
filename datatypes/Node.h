#ifndef _GCM_NODE_H
#define _GCM_NODE_H  1

#define LOCAL 1
#define REMOTE 0
#define UNUSED -1

#define INNER 1
#define BORDER 2

#define FREE 0
#define IN_CONTACT 1

#include "Basis.h"
#include "ContactData.h"

class Node
{
public:
	int zone_num;
	int local_num;
	int remote_num;
	int absolute_num;
	int placement_type; // We need this to store NOT only LOCAL / REMOTE state. We have UNUSED flag also.
	int border_type;
	int contact_type;
	contact_state* contact_data;
	basis* local_basis;
	float coords[3];
	float fixed_coords[3];
protected:
private:
};

#endif
