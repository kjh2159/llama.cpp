#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <string>

std::vector<std::string> parseCSVLine(const std::string& line);
std::vector<std::vector<std::string>> readCSV(const std::string& filename);
std::string replace(std::string origin, std::string target, std::string destination);

#endif // UTILS_H
