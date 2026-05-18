add_library(scorch_game STATIC
    saccessory.c
    saddconf.c
    scffile.c
    scffileold.c
    scolor.c
    sconfig.c
    seconomy.c
    sexplosion.c
    sgame.c
    shelp.c
    shelpdata.c
    sinfo.c
    sinventory.c
    sland.c
    slscape.c
    slscapetools.c
    soptions.c
    sphoenix.c
    sphysics.c
    splayer.c
    spreround.c
    sregistry.c
    sshield.c
    sspill.c
    sstate.c
    stankpro.c
    strack.c
    sweapon.c
)

target_include_directories(scorch_game PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/sutil
    ${CMAKE_SOURCE_DIR}/libj
)
