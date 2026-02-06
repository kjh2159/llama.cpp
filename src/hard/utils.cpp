#include "utils.h"

bool is_csv_file(const std::string & filename) {
    fs::path p(filename);
    if (fs::exists(p) && p.extension() == ".csv") {
        return true;
    }
    return false;
}
bool is_json_file(const std::string & filename) {
    fs::path p(filename);
    if (fs::exists(p) && p.extension() == ".json") {
        return true;
    }
    return false;
}
static std::vector<std::string> parseCSVLine(const std::string& line) {
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

std::vector<std::string> readJSON(const std::string& filename){
    // A parsing function for "questions.json" with very simple way
    // The following is JSON file type:
    // {
    //   "questions": [
    //     "the first content of question",
    //     "the second content of question",
    //     "the third content of question"
    //   ]
    // }
    std::ifstream file(filename);
    std::vector<std::string> qs;
    try {
        json jsonData; file >> jsonData; // JSON parsing

        if (jsonData.contains("questions") && jsonData["questions"].is_array()) {
            for (const auto& item : jsonData["questions"]) {
                if (item.is_string()) { qs.push_back(item.get<std::string>()); }
            }
        } else { std::cerr << "Invalid JSON format: 'data' key missing or not an array\n"; }
    } catch (const std::exception &e) {
        std::cerr << "JSON parsing error: " << e.what() << "\n";
    }
    return qs;
}

std::vector<std::string> loadQuestions(const std::string &filename) {
    std::vector<std::string> questions;
    // csv case
    if (is_csv_file(filename)) {
        // If CSV data is found, extract the second column as questions
        for (const auto& row : readCSV(filename)) {
            if (!row.empty()) questions.push_back(row[1]);
        }
        return questions;
    }

    // json case
    if (is_json_file(filename)) return readJSON(filename);

    // no supported
    std::cerr << "Unsupported file format. Did not read: " << filename << "\n";

    return questions;
}

std::string joinPaths(const std::string& path1, const std::string& path2) {
    if (path1.empty()) return path2;
    if (path2.empty()) return path1;

    char lastChar = path1[path1.length() - 1];
    char firstChar = path2[0];

    if (lastChar == '/' && firstChar == '/') {
        return path1 + path2.substr(1);
    } else if (lastChar != '/' && firstChar != '/') {
        return path1 + "/" + path2;
    } else {
        return path1 + path2;
    }
}

std::string replace(std::string origin, std::string target, std::string destination) {
    size_t pos = 0;
    while ((pos = origin.find(target, pos)) != std::string::npos) {
        origin.replace(pos, target.length(), destination);
        pos += destination.length();
    }
    return origin;
}

std::vector<std::string> split_string(const std::string & str){
    //initialization
    std::vector<std::string> result;
    // conversion to stream string
    std::istringstream iss(str);
    std::string value; // splitted value

    while (iss >> value){
        result.push_back(value);
    }

    return result;
}

std::string execute_cmd(const char* cmd) {
    // command execution
    FILE* pipe = popen(cmd, "r");

    // check pipe open
    if (!pipe) { fprintf(stderr, "failed to pipe open (record.h)\n"); return ""; }

    // get output from buffer
    std::ostringstream result;
    char buff[8192];
    while (fgets(buff, sizeof(buff), pipe) != nullptr) {
        result << buff;
    }

    pclose(pipe);
    return result.str();
}

std::string apply_sudo_and_get(std::string command) {
    std::string cmd = "su -c \"";                                                                // prefix
    if (command != "") cmd += command;
    else cmd += "awk '{print \\$1/1000}' /sys/devices/system/cpu/cpu7/cpufreq/scaling_cur_freq"; // command
    cmd += "\"";                                                                                 // postfix

    return cmd;
}