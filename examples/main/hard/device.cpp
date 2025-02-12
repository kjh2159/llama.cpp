#include "device.h"

Device::Device(const std::string& device_name) : device(device_name){
    if (device_name == "S22_Ultra"){
	cluster_indices = {0, 4, 7};
    } else if (device_name == "S24"){
	cluster_indices = {0,4,7,9};
    } else if (device_name == "Fold_4"){
	cluster_indices = {0, 4, 7};
	}
}

const std::vector<int> Device::get_cluster_indices() const{
    return this->cluster_indices;
}

