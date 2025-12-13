#include <nvml.h>
#include <cstdint>
#include <iostream>

#include "nvgpu.h"

using prometheus::Family;
using prometheus::Gauge;


std::vector<std::unique_ptr<NvGPU>> GetGPUs(
    Family<Gauge>& utilization,
    Family<Gauge>& memory,
    Family<Gauge>& power,
    Family<Gauge>& temperature
) {
    nvmlInit();

    uint32_t deviceCount;
    nvmlDeviceGetCount(&deviceCount);

    std::vector<std::unique_ptr<NvGPU>> gpus;
    for (uint32_t i=0; i<deviceCount; ++i) {
        gpus.push_back(std::make_unique<NvGPU>(i, utilization, memory, power, temperature));
    }
    return gpus;
}


#define GPU_UTILIZATION "gpu_utilization"
#define MEM_UTILIZATION "mem_utilization"
#define DEC_UTILIZATION "dec_utilization"
#define ENC_UTILIZATION "enc_utilization"
#define FAN_UTILIZATION "fan_utilization"
#define MEM_TOTAL "mem_total"
#define MEM_USED "mem_used"
#define MEM_FREE "mem_free"
#define MEM_RESERVED "mem_reserved"
#define POWER_MAX "power_max"
#define POWER_LIMIT "power_limit"
#define POWER_CUR "power_cur"
#define TEMPERATURE "temperature"


NvGPU::NvGPU(uint32_t index,
    Family<Gauge>& utilization,
    Family<Gauge>& memory,
    Family<Gauge>& power,
    Family<Gauge>& temperature)
: m_index(index)
{
    nvmlDeviceGetHandleByIndex(m_index, &m_device);

    char name[NVML_DEVICE_NAME_V2_BUFFER_SIZE];
    nvmlDeviceGetName(m_device, name, NVML_DEVICE_NAME_V2_BUFFER_SIZE);
    m_name = name;

    char uuid[NVML_DEVICE_UUID_V2_BUFFER_SIZE];
    nvmlDeviceGetUUID(m_device, uuid, NVML_DEVICE_UUID_V2_BUFFER_SIZE);
    m_uuid = uuid;

    auto gpu_index = std::to_string(m_index);

    m_gauges[GPU_UTILIZATION] = &utilization.Add({{"gpu", gpu_index}, {"utilization", "gpu"}});
    m_gauges[MEM_UTILIZATION] = &utilization.Add({{"gpu", gpu_index}, {"utilization", "memory"}});
    m_gauges[DEC_UTILIZATION] = &utilization.Add({{"gpu", gpu_index}, {"utilization", "decoder"}});
    m_gauges[ENC_UTILIZATION] = &utilization.Add({{"gpu", gpu_index}, {"utilization", "encoder"}});
    m_gauges[FAN_UTILIZATION] = &utilization.Add({{"gpu", gpu_index}, {"utilization", "fan"}});
    m_gauges[MEM_TOTAL] = &memory.Add({{"gpu", gpu_index}, {"memory", "total"}});
    m_gauges[MEM_USED] = &memory.Add({{"gpu", gpu_index}, {"memory", "used"}});
    m_gauges[MEM_FREE] = &memory.Add({{"gpu", gpu_index}, {"memory", "free"}});
    m_gauges[MEM_RESERVED] = &memory.Add({{"gpu", gpu_index}, {"memory", "reserved"}});;
    m_gauges[POWER_MAX] = &power.Add({{"gpu", gpu_index}, {"power", "max"}});
    m_gauges[POWER_LIMIT] = &power.Add({{"gpu", gpu_index}, {"power", "limit"}});
    m_gauges[POWER_CUR] = &power.Add({{"gpu", gpu_index}, {"power", "current"}});
    m_gauges[TEMPERATURE] = &temperature.Add({{"gpu", gpu_index}});
}


NvGPU::~NvGPU() {}


bool NvGPU::tick() {
    nvmlUtilization_t utilization;
    auto rv = nvmlDeviceGetUtilizationRates(m_device, &utilization);
    if (rv == NVML_SUCCESS) {
        m_gauges[GPU_UTILIZATION]->Set(utilization.gpu);
        m_gauges[MEM_UTILIZATION]->Set(utilization.memory);
    }

    uint32_t sampling_period;
    uint32_t decoder;
    rv = nvmlDeviceGetDecoderUtilization(m_device, &decoder, &sampling_period);
    if (rv == NVML_SUCCESS) {
        m_gauges[DEC_UTILIZATION]->Set(decoder);
    }

    uint32_t encoder;
    rv = nvmlDeviceGetEncoderUtilization(m_device, &encoder, &sampling_period); 
    if (rv == NVML_SUCCESS) {
        m_gauges[ENC_UTILIZATION]->Set(encoder);
    }

    uint32_t fan_speed;
    rv = nvmlDeviceGetFanSpeed(m_device, &fan_speed);
    if (rv == NVML_SUCCESS) {
        m_gauges[FAN_UTILIZATION]->Set(fan_speed);
    }


    nvmlMemory_v2_t memory;
    memory.version = nvmlMemory_v2;
    rv = nvmlDeviceGetMemoryInfo_v2(m_device, &memory);
    if (rv == NVML_SUCCESS) {
        m_gauges[MEM_TOTAL]->Set(memory.total);
        m_gauges[MEM_USED]->Set(memory.used);
        m_gauges[MEM_FREE]->Set(memory.free);
        m_gauges[MEM_RESERVED]->Set(memory.reserved);
    }

    uint32_t power_max;
    rv = nvmlDeviceGetPowerManagementLimit(m_device, &power_max);
    if (rv == NVML_SUCCESS) {
        m_gauges[POWER_MAX]->Set(power_max);
    }

    uint32_t power_limit;
    rv = nvmlDeviceGetEnforcedPowerLimit(m_device, &power_limit);
    if (rv == NVML_SUCCESS) {
        m_gauges[POWER_LIMIT]->Set(power_limit);
    }

    uint32_t power;
    rv = nvmlDeviceGetPowerUsage(m_device, &power);
    if (rv == NVML_SUCCESS) {
        m_gauges[POWER_CUR]->Set(power);
    }

    uint32_t temperature;
    rv = nvmlDeviceGetTemperature(m_device, NVML_TEMPERATURE_GPU, &temperature);
    if (rv == NVML_SUCCESS) {
        m_gauges[TEMPERATURE]->Set(temperature);
    }

    return true;
}
