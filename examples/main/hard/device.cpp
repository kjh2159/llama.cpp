#include "device.h"

Device::Device(const std::string& device_name) : device(device_name){
    if (device_name == "S22_Ultra" || device_name == "Fold4" ||  device_name == "Pixel9"){
	    cluster_indices = {0, 4, 7};
    } else if (device_name == "S24"){
	    cluster_indices = {0, 4, 7, 9};
    }
}

const std::vector<int> Device::get_cluster_indices() const{
    return this->cluster_indices;
}

const std::string Device::get_device_name() const{
    return this->device;
}
