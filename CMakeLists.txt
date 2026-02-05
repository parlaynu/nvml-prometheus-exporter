cmake_minimum_required(VERSION 3.25.1)
include(FetchContent)

project(nvml-prometheus-exporter)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/nvgpu.cpp
)

set(LINK_LIBS
    prometheus-cpp::pull
    CUDA::nvml
) 
if (WIN32)
  list(APPEND LINK_LIBS ws2_32) 
endif()

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    ${LINK_LIBS}
)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
    PROJECT_NAME="${CMAKE_PROJECT_NAME}"
)

if (WIN32)
    install(TARGETS ${PROJECT_NAME}
        DESTINATION bin
        RUNTIME_DEPENDENCIES
        PRE_EXCLUDE_REGEXES "api-ms-"
                            "ext-ms-"
                            "opengl32"
        POST_EXCLUDE_REGEXES ".*system32/.*\\.dll"
        DIRECTORIES $ENV{PATH})
else()
    install(TARGETS ${PROJECT_NAME}
        DESTINATION bin
        RUNTIME_DEPENDENCIES)
endif()

find_package(CUDAToolkit REQUIRED)

set(ENABLE_PUSH OFF)
set(ENABLE_TESTING OFF)
FetchContent_Declare(prometheus-cpp
    GIT_REPOSITORY https://github.com/jupp0r/prometheus-cpp.git
    GIT_TAG v1.3.0
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(prometheus-cpp)

