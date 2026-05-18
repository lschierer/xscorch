cmake_minimum_required(VERSION 3.16)
project(xscorch C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(GNUInstallDirs)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

add_compile_definitions(
    HAVE_CONFIG_H=1
    SC_INSTALL_DATADIR="${CMAKE_INSTALL_FULL_DATADIR}"
    PACKAGE="xscorch"
    VERSION="0.2.1"
)

# Common include paths used throughout the project
set(XSCORCH_INCLUDES
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/sutil
    ${CMAKE_SOURCE_DIR}/libj
    ${GTK3_INCLUDE_DIRS}
)

add_compile_options(${GTK3_CFLAGS_OTHER} -Wall -Wno-deprecated-declarations)
link_directories(${GTK3_LIBRARY_DIRS})

add_subdirectory(libj/jstr)
add_subdirectory(libj/jreg)
add_subdirectory(sutil)
add_subdirectory(sgame)
add_subdirectory(sai)
add_subdirectory(ssound)
add_subdirectory(sgtk/swidgets)
add_subdirectory(sgtk)

add_executable(xscorch xscorch.c)

target_include_directories(xscorch PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/sgame
    ${CMAKE_SOURCE_DIR}/sgtk
)

target_link_libraries(xscorch
    scorch_gtk
    scorch_widget_gtk
    scorch_game
    scorch_ai
    scorch_sound_null
    scorch_util
    jreg
    jstr
    m
    ${GTK3_LIBRARIES}
)

install(TARGETS xscorch RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES
    data/accessories.def
    data/profiles.def
    data/scorings.def
    data/weapons.def
    DESTINATION ${CMAKE_INSTALL_DATADIR}/xscorch
)
install(FILES
    img/xscorch-logo.png
    img/xscorch-icon.png
    DESTINATION ${CMAKE_INSTALL_DATADIR}/xscorch/images
)
