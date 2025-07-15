// #include "record.h"
// #include "dvfs.h"


// #include <thread>

// int main(){
//     // initialization
//     const std::string device_name = "Fold_4";
//     const DVFS dvfs(device_name);
	
//     // get cpu freq
//     for(const auto freqs: dvfs.get_cpu_freq()){
// 	std::cout << "cluster: " << freqs.first << " --- { ";
// 	for (std::size_t i=0; i<freqs.second.size(); ++i){
//     	    std::cout << freqs.second[i] << " ";
// 	}
// 	std::cout << "}\n";
//     }
    
//     // get ram freq
//     std::cout << "DDR Freq: { ";
//     for(const auto freq: dvfs.get_ddr_freq()){
// 	std::cout << freq << " ";
//     }
//     std::cout << "}\n";

// 	// get hard info names
// 	std::string names = get_records_names(dvfs);
// 	std::cout << names << std::endl;

//     // get hard info (without systime)
// 	std::vector<std::string> o = get_hard_records(dvfs); //(not including systime)
// 														 //record_hard function includes systime
    
// 	// get system time
// 	auto now1 = std::chrono::system_clock::now();
// 	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
// 	auto now2 = std::chrono::system_clock::now();
// 	auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now2-now1).count();

// 	// get hard info with systime
// 	std::cout << std::endl;
// 	now1 = std::chrono::system_clock::now();
// 	o = get_hard_records(dvfs);
// 	now2 = std::chrono::system_clock::now();
// 	millis = std::chrono::duration_cast<std::chrono::milliseconds>(now2-now1).count();
// 	o.insert(o.begin(), std::to_string(millis));
// 	for (std::size_t i=0; i<o.size(); ++i) std::cout << o[i] << ", ";
// 	std::cout << "\n" << millis << std::endl;


// 	// write hard_info file
// 	bool sigterm = false;
// 	record_hard(sigterm, dvfs);	
	
	
// 	return 0;
// }
