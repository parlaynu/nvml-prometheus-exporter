# Prometheus Exporter for Nvidia GPUs

Initial simple exporter for Nvidia GPU metrics for Prometheus.

* written in C++ and linking directly to the nvml libraries. 
* using the [prometheus-cpp](https://github.com/jupp0r/prometheus-cpp) library

## Building and Running

Build with cmake:

    $ cmake -S . -B build
    $ cmake --build build
    $ cmake --install build --prefix install 

To run:

    $ ./install/bin/nvml-prometheus-exporter [-l <address:port>]

The default listen address and port are: `0.0.0.0:11011`

## Configuring Prometheus

The scrape config is simple as below.

    scrape_configs:
    - job_name: nvprom
      static_configs:
        - targets:
          - '<ip-address>:11011'

Port `11011` is the default port. This can be changed with the `-l` command line flag
as above.

## Running As A Daemon/Service

On Linux, I just start it with a boot time cron for root. Will probably create a systemd
service file for it one day.

On Windows, I use [nssm](https://nssm.cc/).
