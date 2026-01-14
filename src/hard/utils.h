#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>

std::vector<std::string> parseCSVLine(const std::string& line);
std::vector<std::vector<std::string>> readCSV(const std::string& filename);
std::string joinPaths(const std::string& path1, const std::string& path2);
std::string replace(std::string origin, std::string target, std::string destination);

// TODO: move
std::vector<std::string> split_string(const std::string & str);

// internal static functions
std::string execute_cmd(const char* cmd);

// throttling detection support
std::string apply_sudo_and_get(std::string command);

struct llama_context; // opaque
struct common_params;
bool init_ignite_filename(struct llama_context * ctx);

#endif // UTILS_H
