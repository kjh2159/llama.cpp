#include "dvfs.h"

const std::map<std::string, std::map<int, std::vector<int>>> DVFS::cpufreq = {
    { "S22_Ultra", {
        { 0, { 307200, 403200, 518400, 614400, 729600, 844800, 960000, 1075200, 1171200, 1267200, 1363200, 1478400, 1574400, 1689600, 1785600 } },
        { 4, { 633600, 768000, 883200, 998400, 1113600, 1209600, 1324800, 1440000, 1555200, 1651200, 1766400, 1881600, 1996800, 2112000, 2227200, 2342400, 2419200 } },
        { 7, { 806400, 940800, 1056000, 1171200, 1286400, 1401600, 1497600, 1612800, 1728000, 1843200, 1958400, 2054400, 2169600, 2284800, 2400000, 2515200, 2630400, 2726400, 2822400, 2841600 } }
    }},
    { "S24", {
        { 0, { 400000, 576000, 672000, 768000, 864000, 960000, 1056000, 1152000, 1248000, 1344000, 1440000, 1536000, 1632000, 1728000, 1824000, 1920000, 1959000 } },
        { 4, { 672000, 768000, 864000, 960000, 1056000, 1152000, 1248000, 1344000, 1440000, 1536000, 1632000, 1728000, 1824000, 1920000, 2016000, 2112000, 2208000, 2304000, 2400000, 2496000, 2592000 } },
        { 7, { 672000, 768000, 864000, 960000, 1056000, 1152000, 1248000, 1344000, 1440000, 1536000, 1632000, 1728000, 1824000, 1920000, 2016000, 2112000, 2208000, 2304000, 2400000, 2496000, 2592000, 2688000, 2784000, 2880000, 2900000 } },
        { 9, { 672000, 768000, 864000, 960000, 1056000, 1152000, 1248000, 1344000, 1440000, 1536000, 1632000, 1728000, 1824000, 1920000, 2016000, 2112000, 2208000, 2304000, 2400000, 2496000, 2592000, 2688000, 2784000, 2880000, 2976000, 3072000, 3207000 } }
    }},
	{ "Fold_4", {
		{ 0, { 300000, 441600, 556800, 691200, 806400, 940800, 1056000, 1132800, 1228800, 1324800, 1440000, 1555200, 1670400, 1804800, 1920000, 2016000} },
		{ 4, { 633600, 768000, 883200, 998400, 1113600, 1209600, 1324800, 1440000, 1555200, 1651200, 1766400, 1881600, 1996800, 2112000, 2227200, 2342400, 2457600, 2572800, 2649600, 2745600 } },
		{ 7, { 787200, 921600, 1036800, 1171200, 1286400, 1401600, 1536000, 1651200, 1766400, 1881600, 1996800, 2131200, 2246400, 2361600, 2476800, 2592000, 2707200, 2822400, 2918400, 2995200 } }
	}}
};

const std::map<std::string, std::vector<int>> DVFS::ddrfreq = {
    { "S22_Ultra", { 547000, 768000, 1555000, 1708000, 2092000, 2736000, 3196000 } },
    { "S24", { 421000, 676000, 845000, 1014000, 1352000, 1539000, 1716000, 2028000, 2288000, 2730000, 3172000, 3738000, 4206000 } },
    { "Fold_4", { 547000, 768000, 1555000, 1708000, 2092000, 2736000, 3196000 } }
};


const std::map<std::string, std::vector<std::string>> DVFS::empty_thermal = {
{ "S22_Ultra", { "sdr0-pa0", "sdr1-pa0", "pm8350b_tz", "pm8350b-ibat-lvl0", "pm8350b-ibat-lvl1", "pm8350b-bcl-lvl0", "pm8350b-bcl-lvl1", "pm8350b-bcl-lvl2", "socd", "pmr735b_tz"}},
{ "Fold_4", { "sdr0-pa0", "sdr1-pa0", "pm8350b_tz", "pm8350b-ibat-lvl0", "pm8350b-ibat-lvl1", "pm8350b-bcl-lvl0", "pm8350b-bcl-lvl1", "pm8350b-bcl-lvl2", "socd", "pmr735b_tz", "qcom,secure-non"}},
{ "S24", {}}
};



// consturctor
DVFS::DVFS(const std::string& device_name) : Device(device_name) {}


const std::map<int, std::vector<int>>& DVFS::get_cpu_freq() const {
    return cpufreq.at(device);
}
const std::vector<std::string>& DVFS::get_empty_thermal() const {
    return empty_thermal.at(device);
}

const std::vector<int>& DVFS::get_ddr_freq() const {
    return ddrfreq.at(device);
}

const int DVFS::set_cpu_freq(std::vector<int>& freq_indices){
	// vector size should be same
	if (this->cluster_indices.size() != freq_indices.size()) return false;

	std::string command = "su -c \"";
	for (int i=0; i<this->cluster_indices.size(); ++i){ 
		int idx = freq_indices[i];
		int clk = this->cpufreq.at(this->device).at(this->cluster_indices[i])[idx];
		command += std::string("echo ") + std::to_string(clk)+ std::string(" > /sys/devices/system/cpu/cpufreq/policy") + std::to_string(idx) + std::string("/scaling_max_freq; ");
		command += std::string("echo ") + std::to_string(clk)+ std::string(" > /sys/devices/system/cpu/cpufreq/policy") + std::to_string(idx) + std::string("/scaling_min_freq; ");
	}
	command += "\""; // closing quote
	
	return system(command.c_str());
}
