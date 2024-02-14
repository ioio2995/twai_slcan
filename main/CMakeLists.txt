idf_component_register(SRCS "cdc_driver.cpp" "twai_slcan.cpp" "twai_driver.cpp" "slcan.cpp" "slcan_helper.cpp" "version.c"
                    INCLUDE_DIRS "include"
                    REQUIRES driver led_rgb
                    PRIV_REQUIRES esp_timer) 
find_package(Git)

if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --tags --abbrev=7 --dirty --always
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_REFERENCE
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
else()
    set(GIT_REFERENCE "unknown")
endif()

target_compile_definitions(${COMPONENT_LIB} PRIVATE GIT_REFERENCE="${GIT_REFERENCE}")