add_library(scorch_util STATIC
    md5.c
    randserv.c
    sfractal.c
    shash.c
    shashlist.c
    srand.c
    sslink.c
)

target_include_directories(scorch_util PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/libj
)
