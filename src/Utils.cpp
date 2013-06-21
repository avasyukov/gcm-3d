#include "Utils.h"

gcm::Params::Params(xmlpp::Element* el) {
	xmlpp::Element::AttributeList attrs = el->get_attributes();
	foreach(attr, attrs)
	{
		(*this)[(*attr)->get_name()] = (*attr)->get_value();
	}
}

bool gcm::Params::has(string param) {
	return find(param) != end();
}

bool gcm::Params::paramEquals(string param, string value) {
	if (has(param))
		if ((*this)[param] == value)
			return true;
	return false;
}

void checkStream(fstream &f) {
	if (f.eof() || f.fail() || f.bad()) 
		THROW_INVALID_INPUT("Input file is corrupted");
}

string getAttributeByName(xmlpp::Element::AttributeList attrs, string name) {
	foreach(attr, attrs)
		if ((*attr)->get_name() == name)
			return (*attr)->get_value();
	THROW_INVALID_ARG("Attribute \"" + name + "\" not found in list");
}

void replaceAll(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}