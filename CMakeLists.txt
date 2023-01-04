cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(Sherphy_Miracle VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARD_LIBS OFF)



# ---- Include guards ----
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()


set(SHERPHY_ENGINE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(3RD_DIR "${SHERPHY_ENGINE_ROOT}/3rdparty")
set(vulkan_include ${3RD_DIR}/VulkanSDK/include)

if(WIN32)
    set(vulkan_lib ${3RD_DIR}/VulkanSDK/lib/Win32/vulkan-1.lib)
    set(glslangValidator_executable ${3RD_DIR}/VulkanSDK/bin/Win32/glslangValidator.exe)
    add_compile_definitions("Miracle_VK_LAYER_PATH=${3RD_DIR}/VulkanSDK/bin/Win32")
else()
  message(FATAL_ERROR "Unknown Platform")
endif()

add_subdirectory(3rdparty)

add_subdirectory(MiracleRuntime)

add_subdirectory(Sherphy_Lan)


