cmake_minimum_required(VERSION 3.25.1)
include(FetchContent)

project(nvml-exporter)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)

FetchContent_Declare(prometheus-cpp
    GIT_REPOSITORY https://github.com/jupp0r/prometheus-cpp.git
    GIT_TAG v1.3.0
)
FetchContent_MakeAvailable(prometheus-cpp)

find_package(CUDAToolkit REQUIRED)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/nvgpu.cpp
)
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    prometheus-cpp::pull
    CUDA::nvml
)


