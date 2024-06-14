#pragma once
#include <sstream>
class json_builder {
	private:
		std::stringstream json_stream;

	public:
		json_builder();

		std::string finish();
		std::string str();

		void end_array();
		void end_last_array();
		void end_object();
		void end_last_object();
		void start_object(std::string name);
		void start_array(std::string name);
		void write_property(std::string name, std::string value, bool is_string_value);
		void write_property(std::string value, bool is_string_value);
		void write_last_property(std::string name, std::string value, bool is_string_value);
		void write_last_property(std::string value, bool is_string_value);
		void start_object();
		void start_array();

};

