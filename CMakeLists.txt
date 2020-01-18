project(kfuseiso)
cmake_minimum_required(VERSION 2.6)

find_package(Qt4 REQUIRED)
find_package(KDE4 REQUIRED)
find_package(LibKonq REQUIRED)

include_directories(${QT_INCLUDES} ${KDE4_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

set(kfuseisomount_SRCS
    fuseisolib.cpp
    kfuseisomount.cpp
)

set(kio_isomedia_SRCS
    fuseisolib.cpp
    kio_isomedia.cpp
)

set(iso_image_plugin_SRCS
    fuseisolib.cpp
    iso_image_plugin.cpp
)

qt4_automoc(${kfuseisomount_SRCS})

add_executable(kfuseisomount ${kfuseisomount_SRCS})
kde4_add_plugin(kio_isomedia ${kio_isomedia_SRCS})
kde4_add_plugin(iso_image_plugin WITH_PREFIX ${iso_image_plugin_SRCS})

target_link_libraries(kfuseisomount ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} ${KDE4_KDE3SUPPORT_LIBS})
target_link_libraries(kio_isomedia ${KDE4_KIO_LIBS} ${KDE4_KDE3SUPPORT_LIBS})
target_link_libraries(iso_image_plugin ${KDE4_KIO_LIBS} ${LIBKONQ_LIBRARY} ${KDE4_KDE3SUPPORT_LIBS})

install(TARGETS kfuseisomount DESTINATION ${BIN_INSTALL_DIR})
install(FILES kfuseisomount.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES media.directory mount.desktop DESTINATION ${DATA_INSTALL_DIR}/kfuseiso)

install(TARGETS kio_isomedia DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES isomedia.protocol DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES x-iso-image.desktop DESTINATION ${MIME_INSTALL_DIR}/application)
install(FILES x-iso-image-mounted.desktop DESTINATION ${MIME_INSTALL_DIR}/inode)

install(TARGETS iso_image_plugin DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES iso_image_plugin.desktop DESTINATION ${SERVICES_INSTALL_DIR})
