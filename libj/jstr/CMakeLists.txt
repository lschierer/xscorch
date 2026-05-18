add_library(jstr STATIC
    str_arg.c
    str_case.c
    str_cgi.c
    str_comp.c
    str_con.c
    str_copy.c
    str_creat.c
    str_info.c
    str_io.c
    str_num.c
    str_printf.c
    str_replace.c
    str_time.c
    str_trim.c
    str_uni.c
)

target_include_directories(jstr PUBLIC
    ${CMAKE_SOURCE_DIR}/libj
    ${CMAKE_CURRENT_SOURCE_DIR}
)
