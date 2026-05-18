add_library(scorch_ai STATIC
    sai.c
    saibuy.c
    saitraj.c
    saiturn.c
)

target_include_directories(scorch_ai PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/sgame
    ${CMAKE_SOURCE_DIR}/sutil
    ${CMAKE_SOURCE_DIR}/libj
)
