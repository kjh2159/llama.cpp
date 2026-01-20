
#ifndef RECORD
#define RECORD

#include "dvfs.h"
#include "utils.h"

#include <stdlib.h>

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <memory>
#include <chrono>
#include <atomic>
#include <thread>

#define HARD_RECORD_FILE "./data/hard_info_termux.txt"
#define INFER_RECORD_FILE "./data/infer_info.csv"
#define TIME_T std::chrono::system_clock::time_point

//test
void get_cpu_info();

// get function
const std::string get_records_names(const DVFS& dvfs);
std::vector<std::string> get_hard_records(const DVFS& dvfs);
std::vector<std::string> get_hard_records_wo_systime(const DVFS& dvfs);

// write function
void write_file(const std::vector<std::string>& data, std::string output);
void write_file(const std::string& data, std::string output);
void record_hard(std::atomic<bool>& sigterm, const DVFS& dvfs); 

#endif
