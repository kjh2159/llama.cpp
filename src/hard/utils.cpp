#include "utils.h"
#include <fstream>
#include <sstream>
#include <iostream>

std::vector<std::string> parseCSVLine(const std::string& line) {
    std::vector<std::string> values;
    std::string current;
    bool insideQuotes = false;

    for (char ch : line) {
        if (ch == '"') {
            insideQuotes = !insideQuotes;
        } else if (ch == ',' && !insideQuotes) {
            values.push_back(current);
            current.clear();
        } else {
            current += ch;
        }
    }
    values.push_back(current); // last field

    return values;
}

std::vector<std::vector<std::string>> readCSV(const std::string& filename) {
    std::vector<std::vector<std::string>> result;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "cannot open file: " << filename << std::endl;
        return result;
    }

    std::string line; std::size_t i = 0;
    while (std::getline(file, line)) {
        result.push_back(parseCSVLine(line));
    }

    file.close();
    return result;
}

std::string replace(std::string origin, std::string target, std::string destination) {
    size_t pos = 0;
    while ((pos = origin.find(target, pos)) != std::string::npos) {
        origin.replace(pos, target.length(), destination);
        pos += destination.length();
    }
    return origin;
}