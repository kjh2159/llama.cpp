#ifndef DVFS_H
#define DVFS_H

#include "device.h"

#include <stdlib.h>

#include <map>


class DVFS : public Device {
private:
    static const std::map<std::string, std::map<int, std::vector<int>>> cpufreq;
    static const std::map<std::string, std::vector<int>> ddrfreq;
    static const std::map<std::string, std::vector<std::string>> empty_thermal;

public:
    explicit DVFS(const std::string& device_name);

    const std::map<int, std::vector<int>>& get_cpu_freq() const;
    const std::vector<int>& get_ddr_freq() const;
	const std::vector<std::string>& get_empty_thermal() const;

	const int set_cpu_freq(std::vector<int>&);
};

#endif //DVFS_H
