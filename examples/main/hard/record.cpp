#include "record.h"
#include <algorithm>


static std::vector<std::string> split_string(const std::string& str){
    // initialization
    std::vector<std::string> result;
    // conversion to stream string
    std::istringstream iss(str);
    std::string value; // splited value

    while (iss >> value){
        result.push_back(value); // accumulation
    }

    return result;
}


static std::string execute_cmd(const char* cmd){
    // command execution
    FILE* pipe = popen(cmd, "r");
    
    // check pipe open
    if (!pipe) { std::cerr << "failed to pipe open (record.h)\n"; return "";}

    // get output from buffer
    std::ostringstream result;
    char buff[8192];
    while (fgets(buff, sizeof(buff), pipe) != nullptr){
        result << buff;
    }

    // close pipe
    pclose(pipe); 
    return result.str();
}

// test function
void get_cpu_info() {
    std::string command = "su -c \""; //prefix
    
    // command to get cpu freq
    command += "awk '{print \\$1/1000}' /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq; ";
    command += "awk '{print \\$1/1000}' /sys/devices/system/cpu/cpu4/cpufreq/scaling_cur_freq; ";
    command += "\""; // postfix

    // only execution
    system(command.c_str());
    //std::string output = execute_cmd(command.c_str());
    //std::cout << execute_cmd(command.c_str())[0] << std::endl;
}



const std::string get_records_names(const DVFS& dvfs) {
    std::string names = "Time,";
    
    // thermal info
    std::string command = "su -c \"cat /sys/devices/virtual/thermal/thermal_zone*/type\"";
    std::string temp_record = execute_cmd(command.c_str());
    std::replace(temp_record.begin(), temp_record.end(), '\n', ',');
    names += temp_record;

    // gpu info
    names += "gpu_min_clock,gpu_max_clock,";

    // cpu info
    for(const auto index : dvfs.get_cluster_indices()){
        names += std::string("cpu") + std::to_string(index) + std::string("_max_freq,cpu") + std::to_string(index) + "_cur_freq,";
    }

    // mem info
    command = "su -c \"awk '{print \\$1}' /proc/meminfo\"";
    temp_record = execute_cmd(command.c_str());
    std::replace(temp_record.begin(), temp_record.end(), ':', '\0');
    std::replace(temp_record.begin(), temp_record.end(), '\n', ',');
    names += temp_record;

    // power
    names += "power_now,";

    // RAM clock info
    names += "scaling_devfreq_max,scaling_devfreq_min,cur_freq";

    // remove emptyThermal 
	for (std::string empty : dvfs.get_empty_thermal()){
		if (empty == "qcom,secure-non"){
			std::size_t p = 0;
			while( (p = names.find(empty, p)) != std::string::npos ){
				names.replace(p, empty.length(), "secure-non");
			}
		    continue;
		}
		std::string temp = empty + ",";
		std::size_t pos = 0;
		while( (pos = names.find(temp, pos)) != std::string::npos){
			names.replace(pos, temp.length(), ""); // string replace
		}
	}

	return names;
}


/* 
 * GET HARD RECORDS function
 * - args 
 *   	- cluster_indices: an integer vector or array to contain cluster indices (ex. {0,4,7})
 * - task
 *   	- Get hard records such as thermal, power, etc.
 * - return
 *   	- A string vector to contain outputs
 * */
std::vector<std::string> get_hard_records(const DVFS& dvfs) {
    std::vector<int> cluster_indices = dvfs.get_cluster_indices();

    std::string command = "su -c \""; // prefix
    
    // thermal info
    command += "awk '{print \\$1/1000}' /sys/devices/virtual/thermal/thermal_zone*/temp; ";
	      
	// GPU clock info
	command += "awk '{print \\$1}' /sys/kernel/gpu/gpu_min_clock; awk '{print \\$1}' /sys/kernel/gpu/gpu_max_clock; ";

    // CPU clock info
    for (std::size_t i=0; i<cluster_indices.size(); ++i){
        command += std::string("awk '{print \\$1/1000}' /sys/devices/system/cpu/cpu")+std::to_string(i)+std::string("/cpufreq/scaling_max_freq; ") +
		   std::string("awk '{print \\$1/1000}' /sys/devices/system/cpu/cpu")+std::to_string(i)+std::string("/cpufreq/scaling_cur_freq; ");
    }

	// RAM info
    command += "awk '{print \\$2/1024}' /proc/meminfo; ";

    // Power Consumption info
    command += "awk '{print}' /sys/class/power_supply/battery/power_now; ";

    // RAM clock info (only S24)
    /* require to be implemented */    



    // closing quote
    command += "\"";
    // execute command and get output
    std::string output = execute_cmd(command.c_str());
    

	//std::cout << command << std::endl; // test
    // string post-processing
    return split_string(output);
}



void write_file(const std::vector<std::string>& data){
	// open file append mode
	std::ofstream file(HARD_RECORD_FILE, std::ios::app);

	// check file open
	if (!file){
		std::cerr << "failed to open file: " << HARD_RECORD_FILE << std::endl;
		return;
	}

	// wrtie file
	for (const auto v : data){
		file << v << ",";
	}
	file << "\n";

	// close file
	file.close();
}

void write_file(const std::string& data){
	// open file append mode
	std::ofstream file(HARD_RECORD_FILE, std::ios::app);

	// check file open
	if (!file){
		std::cerr << "failed to open file: " << HARD_RECORD_FILE << std::endl;
		return;
	}

	// wrtie file
	file << data << "\n";

	// close file
	file.close();
}



/*
 * TO DO
 * ### This function should be called by background process! ###
 * 
 * ### sigterm should be true after experiment completion
 * 
 * */
void record_hard(bool& sigterm, const DVFS& dvfs){

    sigterm = false;

	// insert hard names
	write_file(get_records_names(dvfs));

	
	int test_index = 0;
	std::vector<std::string> records;
    auto start_sys_time = std::chrono::system_clock::now();
    do{
		auto now = std::chrono::system_clock::now();
		records = get_hard_records(dvfs);
		auto sys_time = std::chrono::duration_cast<std::chrono::milliseconds>(now-start_sys_time).count(); // ms base
		records.insert(records.begin(), std::to_string(sys_time)); // insert systime into firstrecord element
		
		// File write record
		write_file(records);

		// tester code: start
		test_index++;
		if (test_index == 3) sigterm = true;
		// tester code: end

    }while(sigterm != true);
}
