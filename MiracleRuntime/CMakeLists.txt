set(TARGET_NAME MiracleRuntime)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE SOURCES "*.cpp")

# add_library(${TARGET_NAME} ${HEADERS} ${SOURCES})
add_executable(${TARGET_NAME} ${HEADERS} ${SOURCES})

# 整理目录
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADERS} ${SOURCES})

set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17)
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Miracle")

target_link_libraries(${TARGET_NAME} PRIVATE tinyobjloader)

target_link_libraries(${TARGET_NAME} PUBLIC glfw)
target_link_libraries(${TARGET_NAME} PUBLIC imgui)
target_link_libraries(${TARGET_NAME} PUBLIC ${vulkan_lib})



target_include_directories(
  ${TARGET_NAME} 
  PUBLIC $<BUILD_INTERFACE:${SHERPHY_ENGINE_ROOT}>
  $<INSTALL_INTERFACE:include/${TARGET_NAME}-${PROJECT_VERSION}>
  ${CMAKE_CURRENT_SOURCE_DIR}
)