


set(SPV_FILE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/public/SherphyShaderLib/SPV)
file(GLOB_RECURSE GLSL_SHADERS ${CMAKE_CURRENT_SOURCE_DIR}/public/SherphyShaderLib/GLSL/*)

message(WARNING ${GLSL_SHADERS})

include(cmake/compileShader.cmake)
compileShader(
    "${glslc_executable}"
    "${GLSL_SHADERS}"
)