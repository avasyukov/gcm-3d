#include "Stresser.h"

Stresser::Stresser()
{
	stresser_type.assign("Generic stresser");
};

Stresser::~Stresser() { };

string* Stresser::get_stresser_type()
{
	return &stresser_type;
};