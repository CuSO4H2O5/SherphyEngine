cmake_minimum_required(VERSION 3.21)

project(SimpleRayTracer)

file(GLOB_RECURSE SRC_FILE *.cpp *.h) 

add_executable(SimpleRayTracer ${SRC_FILE})

target_include_directories(SimpleRayTracer
    PRIVATE
        I:/SherphyEngine/MiracleRuntime/Soul
    )

    