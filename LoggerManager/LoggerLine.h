#pragma once
#include <string>

struct LoggerLine {
	LoggerLine() {};
	LoggerLine(std::string line)
	{
		size_t find_index;

		find_index = line.find("-");
		m_thread_id = line.substr(0, find_index);
		line.erase(0, find_index + 1);

		find_index = line.find("-");
		m_data_time = line.substr(0, find_index);
		line.erase(0, find_index + 1);

		find_index = line.find("-");
		m_log_level = line.substr(1, find_index - 2);
		line.erase(0, find_index + 1);

		find_index = line.find(" ");
		m_log_place = line.substr(1, find_index - 2);
		line.erase(0, find_index + 1);

		m_message = line;
	}
	std::string get_thread_id() { return m_thread_id; }
	std::string get_data_time() { return m_data_time; }
	std::string get_log_level() { return m_log_level; }
	std::string get_log_place() { return m_log_place; }
	std::string get_message() { return m_message; }
	size_t IsIn(const std::string& text) const
	{
		return ((m_thread_id + "-"
			+ m_data_time + "-"
			+ m_log_level + "-"
			+ m_log_place + " "
			+ m_message).find(text) != std::string::npos);
	}
private:
	std::string m_thread_id;
	std::string m_data_time;
	std::string m_log_level;
	std::string m_log_place;
	std::string m_message;
};

