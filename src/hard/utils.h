#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>

// #include "nlohmann/json.hpp"
#include "nlohmann/json.hpp"

// TODO: move to files.h/.cpp
// File utils
namespace fs = std::filesystem;
using json = nlohmann::json;
bool is_csv_file(const std::string & filename);
bool is_json_file(const std::string & filename);
static std::vector<std::string> parseCSVLine(const std::string& line);
std::vector<std::vector<std::string>> readCSV(const std::string& filename);
std::vector<std::string> readJSON(const std::string& filename);
std::vector<std::string> loadQuestions(const std::string &filename);
std::string joinPaths(const std::string& path1, const std::string& path2);

// string utils
std::vector<std::string> split_string(const std::string & str);
std::string replace(std::string origin, std::string target, std::string destination);

// internal static functions
std::string execute_cmd(const char* cmd);

// throttling detection support
std::string apply_sudo_and_get(std::string command);

#endif // UTILS_H
