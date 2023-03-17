
macro(compileShader COMPILE_BIN SHADERS)
    set(working_dir "${CMAKE_CURRENT_SOURCE_DIR}")
    set(ALL_GENERATED_SPV_FILES "")

    foreach(SHADER ${SHADERS})
        message(WARNING ${SHADER})
        get_filename_component(SHADER_NAME ${SHADER} NAME)
        string(REPLACE "." "_" HEADER_NAME ${SHADER_NAME})
        string(TOUPPER ${HEADER_NAME} GLOBAL_SHADER_VAR)

        set(SPV_FILE "${SPV_FILE_DIR}/${SHADER_NAME}.spv")

        add_custom_command(
            OUTPUT ${SPV_FILE}
            COMMAND ${COMPILE_BIN} ${SHADER} -o ${SPV_FILE}
            DEPENDS ${SHADER}
            WORKING_DIRECTORY "${working_dir}"
        )
        list(APPEND ALL_GENERATED_SPV_FILES ${SPV_FILE})

    endforeach()
endmacro(compileShader)

