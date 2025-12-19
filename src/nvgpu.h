#pragma once

#include <prometheus/family.h>
#include <prometheus/gauge.h>

#include <nvml.h>
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>


class NvGPU {
public:
    NvGPU(uint32_t index, 
            prometheus::Family<prometheus::Gauge>& utilization,
            prometheus::Family<prometheus::Gauge>& memory,
            prometheus::Family<prometheus::Gauge>& power,
            prometheus::Family<prometheus::Gauge>& temperature);
    ~NvGPU();

    uint32_t index() const { return m_index; }
    std::string name() const { return m_name; }
    std::string uuid() const { return m_uuid; }

    bool tick();

private:
    uint32_t m_index;
    std::string m_name;
    std::string m_uuid;
    nvmlDevice_t m_device;

    std::unordered_map<std::string, prometheus::Gauge*> m_gauges;
};

std::vector<std::unique_ptr<NvGPU>> GetGPUs(
    prometheus::Family<prometheus::Gauge>& utilization,
    prometheus::Family<prometheus::Gauge>& memory,
    prometheus::Family<prometheus::Gauge>& power,
    prometheus::Family<prometheus::Gauge>& temperature
);

