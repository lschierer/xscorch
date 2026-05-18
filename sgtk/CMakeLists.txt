add_library(scorch_gtk STATIC
    sai-gtk.c
    sautodef-gtk.c
    scolor-gtk.c
    seconomy-gtk.c
    send-gtk.c
    sexplosion-gtk.c
    sfile-gtk.c
    sfont-gtk.c
    sgraphics-gtk.c
    simage-gtk.c
    sinventory-gtk.c
    sland-gtk.c
    slottery-gtk.c
    smainmenu-gtk.c
    smenu-gtk.c
    snet-gtk.c
    soptions-gtk.c
    spaint-gtk.c
    sphysics-gtk.c
    splayer-gtk.c
    ssound-gtk.c
    sspill-gtk.c
    sstatus-gtk.c
    ssystem-gtk.c
    stank-gtk.c
    sweapons-gtk.c
    swindow-gtk.c
)

target_include_directories(scorch_gtk PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/sgtk/swidgets
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/sgame
    ${CMAKE_SOURCE_DIR}/sai
    ${CMAKE_SOURCE_DIR}/sutil
    ${CMAKE_SOURCE_DIR}/libj
    ${GTK3_INCLUDE_DIRS}
)

target_compile_options(scorch_gtk PRIVATE ${GTK3_CFLAGS_OTHER})
target_link_libraries(scorch_gtk PRIVATE scorch_widget_gtk ${GTK3_LIBRARIES})
