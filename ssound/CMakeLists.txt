add_library(scorch_sound_null STATIC
    ssoundnull.c
)

target_include_directories(scorch_sound_null PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/libj
)
