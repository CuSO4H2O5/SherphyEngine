
set(TARGET_NAME Miracle_Runtime)
file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE SOURCES "*.cpp")
# add_library(${TARGET_NAME} ${HEADERS} ${SOURCES})
add_executable(${TARGET_NAME} ${HEADERS} ${SOURCES})

set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Miracle")

# precompile headers
file(GLOB_RECURSE PRECOMPILE_HEADERS "./Soul/PreCompile/*.h")
if(Sherphy_CMAKE_DEBUG)
  message(WARNING "${PRECOMPILE_HEADERS}")
endif()
target_precompile_headers(${TARGET_NAME} PUBLIC ${PRECOMPILE_HEADERS})

# 整理目录
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADERS} ${SOURCES})

target_link_libraries(${TARGET_NAME} PUBLIC stb)
target_link_libraries(${TARGET_NAME} PUBLIC json11)
target_link_libraries(${TARGET_NAME} PRIVATE tinyobjloader)
target_link_libraries(${TARGET_NAME} PUBLIC EASTL)
target_link_libraries(${TARGET_NAME} PUBLIC volk)
target_link_libraries(${TARGET_NAME} PUBLIC glfw)
target_link_libraries(${TARGET_NAME} PUBLIC imgui)
target_link_libraries(${TARGET_NAME} PUBLIC ${vulkan_lib})

target_include_directories(
  ${TARGET_NAME} 
  PUBLIC $<BUILD_INTERFACE:${SHERPHY_ENGINE_ROOT}>
  $<INSTALL_INTERFACE:include/${TARGET_NAME}-${PROJECT_VERSION}>
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${glm_DIR}
)

target_compile_definitions(${TARGET_NAME} PUBLIC VK_NO_PROTOTYPES)
