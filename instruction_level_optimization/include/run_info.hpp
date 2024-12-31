#pragma once

#include <vector>
#include <string>

struct RunInfo {
    std::string name;
    int latency_ms; 
    std::vector<std::string> attr_names;
    std::vector<std::string> attr_values;
};

std::ostream &operator<<(std::ostream &os, const std::vector<std::string> &values);

std::ostream &operator<<(std::ostream &os, const RunInfo &run_info);
