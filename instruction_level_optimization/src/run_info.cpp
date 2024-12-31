#include <vector>
#include <string>
#include <iostream>

#include "run_info.hpp"

std::ostream &operator<<(std::ostream &os, const std::vector<std::string> &values) {
    os << "[";
    for (size_t i = 0; i + 1 < values.size(); ++i) {
        os << "\"" << values[i] << "\", ";
    }
    if (values.size() > 0) {
        os << "\"" << values[values.size() - 1] << "\"";
    }
    os << "]";
    return os;
}

std::ostream &operator<<(std::ostream &os, const RunInfo &run_info) {
    os << "{\n";
    os << "  \"name\": \"" << run_info.name << "\",\n";
    os << "  \"latency_ms\": " << run_info.latency_ms << ",\n";
    os << "  \"attr_names\": " << run_info.attr_names << ",\n";
    os << "  \"attr_values\": " << run_info.attr_values << "\n";
    os << "}";
    return os;
}
