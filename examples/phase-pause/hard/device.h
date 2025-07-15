#ifndef DEVICE_H
#define DEVICE_H

#include <string>
#include <vector>
#include <iostream>

class Device {
protected:
    std::string device;
    std::vector<int> cluster_indices;

public:
    Device() = delete;
    explicit Device(const std::string& device_name);

    const std::vector<int> get_cluster_indices() const;
    const std::string get_device_name() const;
};

#endif // DEVICE_HPP
