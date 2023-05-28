cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(Sherphy_Miracle VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARD_LIBS OFF)

# --- Include guards ---
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

# --- Set Directories ---
set(SHERPHY_ENGINE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(3RD_DIR "${SHERPHY_ENGINE_ROOT}/3rdparty")
set(vulkan_install_dir ${3RD_DIR}/VulkanSDK)
set(vulkan_include ${vulkan_install_dir}/include)

# --- Options ---
option(UsingEASTL "If using eastl to faster this project development" ON)
option(Sherphy_CMAKE_DEBUG "Take on CMake Debug Message" OFF)
option(SHERPHY_RUNTIME_DEBUG "Runtime SHERPHY_DEBUG" OFF)

# --- Definations ---
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(WIN64 1)
  ADD_DEFINITIONS(-DWIN64)
endif()

if(SHERPHY_RUNTIME_DEBUG)
  ADD_DEFINITIONS(-DSHERPHY_DEBUG)
endif()

# --- Add Lib Path ---
set(glm_DIR ${3RD_DIR}/glm/glm)
if(WIN32)
  if(WIN64)
    set(vulkan_lib ${3RD_DIR}/VulkanSDK/lib/Win64/vulkan-1.lib)
    set(glslangValidator_executable ${3RD_DIR}/VulkanSDK/bin/glslangValidator.exe)
    set(glslc_executable ${3RD_DIR}/VulkanSDK/bin/glslc.exe)
    add_compile_definitions("Miracle_VK_LAYER_PATH=${3RD_DIR}/VulkanSDK/bin/Win64")
  else()
    message(FATAL_ERROR "WIN64 Not Set")
  endif()
else()
  message(FATAL_ERROR "Unknown Platform")
endif()


add_subdirectory(3rdparty)
add_subdirectory(MiracleRuntime)
add_subdirectory(Sherphy_Lan)
add_subdirectory(resource)