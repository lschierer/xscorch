add_library(jreg STATIC
    reg.c
    regblock.c
    regio.c
    regvar.c
)

target_include_directories(jreg PUBLIC
    ${CMAKE_SOURCE_DIR}/libj
    ${CMAKE_SOURCE_DIR}/libj/jstr
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(jreg PRIVATE jstr)
