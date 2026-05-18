add_library(scorch_widget_gtk STATIC
    sactiveconsole.c
    sactoggle.c
    sconsole.c
    sdialog.c
    sdisplay.c
    sdrawbuf.c
    slabel.c
    slinkcheck.c
    slinkcombo.c
    slinkentry.c
    slinkspin.c
)

target_include_directories(scorch_widget_gtk PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/sgtk
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/sutil
    ${CMAKE_SOURCE_DIR}/libj
    ${GTK3_INCLUDE_DIRS}
)

target_compile_options(scorch_widget_gtk PRIVATE ${GTK3_CFLAGS_OTHER})
