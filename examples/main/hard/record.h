
#ifndef RECORD
#define RECORD

#include "dvfs.h"

#include <stdlib.h>

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <memory>
#include <chrono>

#define HARD_RECORD_FILE "/sdcard/Documents/hard_info.txt"



// internal static functions
static std::vector<std::string> split_string(const std::string& str);
static std::string execute_cmd(const char* cmd);

//test
void get_cpu_info();

// get function
const std::string get_records_names(const DVFS& dvfs);
std::vector<std::string> get_hard_records(const DVFS& dvfs);

// write function
void write_file(const std::vector<std::string>& data);
void write_file(const std::string& data);
void record_hard(bool& sigterm, const DVFS& dvfs); // need to be implemeented

#endif
