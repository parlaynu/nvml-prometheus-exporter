# Prometheus Exporter for Nvidia GPUs

Initial simple exporter for Nvidia GPU metrics for Prometheus.

* written in C++ and linking directly to the nvml libraries. 
* using the [prometheus-cpp](https://github.com/jupp0r/prometheus-cpp) library

To come:
* configuration
* logging
* documentation

## Building and Running

Build with cmake:

    $ cmake -S . -B build
    $ cmake --build build


To run:

    $ ./build/nvprom-exporter

## Configuring Prometheus

The scrape config is simple as below.

    scrape_configs:
    - job_name: nvprom
      static_configs:
        - targets:
          - '<ip-address>:8080'

At the moment, port `8080` is hard coded in the source. This will be configurable
at some point.

