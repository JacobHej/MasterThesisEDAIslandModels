#include "json_builder.h"

json_builder::json_builder() {
	json_stream << "{" << std::endl;
}

std::string json_builder::finish() {
	json_stream << "}" << std::endl;
	return json_stream.str();
}

std::string json_builder::str() {
	return json_stream.str();
}

void json_builder::start_array(std::string name) {
	json_stream << "\"" << name << "\"" << ": " << "[" << std::endl;
}

void json_builder::start_array() {
	json_stream << "[" << std::endl;
}

void json_builder::end_array() {
	json_stream << "]," << std::endl;
}

void json_builder::end_last_array() {
	json_stream << "]" << std::endl;
}

void json_builder::start_object(std::string name) {
	json_stream << "\"" << name << "\"" << ": " << "{" << std::endl;
}
void json_builder::start_object() {
	json_stream << "{" << std::endl;
}

void json_builder::end_object() {
	json_stream << "}," << std::endl;
}

void json_builder::end_last_object() {
	json_stream << "}" << std::endl;
}

void json_builder::write_property(std::string name, std::string value, bool is_string_value) {
	if (is_string_value) json_stream << "\"" << name << "\": " << "\"" << value << "\"," << std::endl;
	else json_stream << "\"" << name << "\": " << value << "," << std::endl;
}

void json_builder::write_property(std::string value, bool is_string_value) {
	if (is_string_value) json_stream << "\"" << value << "\"," << std::endl;
	else json_stream << value << ",";// << std::endl;
}

void json_builder::write_last_property(std::string name, std::string value, bool is_string_value) {
	if (is_string_value) json_stream << "\"" << name << "\": " << "\"" << value << "\"" << std::endl;
	else json_stream << "\"" << name << "\": " << value << std::endl;
}

void json_builder::write_last_property(std::string value, bool is_string_value) {
	if (is_string_value) json_stream << "\"" << value << "\"" << std::endl;
	else json_stream << value << "" << std::endl;
}

