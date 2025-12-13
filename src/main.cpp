#include "nvgpu.h"

#include <prometheus/exposer.h>
#include <prometheus/registry.h>
#include <prometheus/family.h>
// #include <prometheus/info.h>

#include <chrono>
#include <thread>
#include <memory>
#include <string>
#include <iostream>

using prometheus::Exposer;
using prometheus::Registry;
using prometheus::BuildGauge;

int main() {

    // create a http server to expose the metrics to prometheus
    Exposer exposer{"0.0.0.0:8080"};

    // create registry
    auto registry = std::make_shared<Registry>();

    // create all the metric families
    auto& gpu_utilization = BuildGauge()
                .Name("gpu_utilization")
                .Register(*registry);

    auto& gpu_memory = BuildGauge()
                .Name("gpu_memory")
                .Register(*registry);

    auto& gpu_power = BuildGauge()
                .Name("gpu_power")
                .Register(*registry);

    auto& gpu_temperature = BuildGauge()
                .Name("gpu_temperature")
                .Register(*registry);

    // get all the GPUs in the system
    auto gpus = GetGPUs(gpu_utilization, gpu_memory, gpu_power, gpu_temperature);

    // ask the exposer to scrape the registry on incoming HTTP requests
    exposer.RegisterCollectable(registry);

    // the main loop
    for (;;) {
        for (auto& gpu: gpus) {
            if (gpu->tick() == false) {
                std::cout << "  ** scrape failed **" << std::endl;
                continue;
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    return 0;
}
