#include "util.h"
#include "map.h"
#include <ctime>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <unistd.h>

std::string now()
{
	time_t t = time(0);
	std::string s = ctime(&t);
	return s.replace(s.find('\n'), 1, " ");
}

void log_control(std::ostream * out_stream, const std::string & message)
{
	static std::ostream * out = &std::clog;
	if(out_stream) {
		out = out_stream;
	}
	if(out && !message.empty()) {
		*out << message << std::endl;
	}
}

void direct_log(std::ostream * out_stream)
{
	log_control(out_stream, "Log started: " + now());
}

void log(const std::string & message)
{
	log_control(0, message);
}


std::string to_string(int value)
{
    std::ostringstream value_out;
    value_out << value;
    return value_out.str();
}

std::string to_string(char value)
{
    std::ostringstream value_out;
    value_out << value;
    return value_out.str();
}

std::string to_string(unsigned value)
{
    std::ostringstream value_out;
    value_out << value;
    return value_out.str();
}

std::string to_string(long unsigned value)
{
    std::ostringstream value_out;
    value_out << value;
    return value_out.str();
}

std::string to_string(const std::string & value)
{
    return value;
}

std::string to_string(const char * value)
{
	return value;
}

void subs_arg_str(std::string & result, int index, const std::string & value)
{
	std::ostringstream out;
	out << "{" << index << "}";
	std::string placeholder = out.str();
	size_t pos = result.find(placeholder);
	while(pos != std::string::npos) {
		result.replace(pos, placeholder.size(), value);
		pos = result.find(placeholder, pos + 1);
	}
}

void delay(int msec)
{
	usleep(msec * 1000);
}

std::vector<std::string> & operator<<(std::vector<std::string> & out, const char * t)
{
	out.push_back(t);
	return out;
}
