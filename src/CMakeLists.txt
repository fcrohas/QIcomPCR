project(qicompcr-binary)

option(WITH_OPENGL "Build OPENGL" ON)
option(WITH_ACARS "Build with licensed ACARS Library" ON)
option(WITH_FFTW "Build with Fast Fourrier Transform Library" ON)
option(WITH_PULSEAUDIO "Build with PulseAudio support" OFF)
option(WITH_PORTAUDIO "Build with PortAudio support" ON)

find_package(Qt4 REQUIRED)
if(WITH_OPENGL)
    find_package(OpenGL REQUIRED)
endif(WITH_OPENGL)

include(${QT_USE_FILE})
#set(QT_LIBRARY_DIR "/usr/lib")

if(WIN32)
    add_definitions(-D_USE_MATH_DEFINES)
endif(WIN32)

if(UNIX)
    set(qicompcr_SRCS ${qicompcr_SRCS} ctype.c)
endif(UNIX)


if(WITH_PULSEAUDIO)
    set(qicompcr_SRCS ${qicompcr_SRCS} CPulseSound.cpp)
    add_definitions(-DWITH_PULSEAUDIO)
endif(WITH_PULSEAUDIO)

if(WITH_PORTAUDIO)
    set(qicompcr_SRCS ${qicompcr_SRCS} CPortAudio.cpp)
    add_definitions(-DWITH_PORTAUDIO)
endif(WITH_PORTAUDIO)


if(WITH_ACARS)
    set(qicompcr_SRCS ${qicompcr_SRCS} CAcars.cpp)
    add_definitions(-DWITH_ACARS)
endif(WITH_ACARS)

if(WITH_FFTW)
     add_definitions(-DFFTW)
endif(WITH_FFTW)

set(qicompcr_SRCS
    main.cpp
    CAcarsGPL.cpp
    CApplication.cpp
    CCommand.cpp
    CDebugWindow.cpp
    CDemodulator.cpp
    CDevicePCR2500.cpp
    CFft.cpp
    CLcdWidget.cpp
    CMainWindow.cpp
    CMorse.cpp
    CPogsag.cpp
    CRemoteControl.cpp
    CSound.cpp
    CSpectrumWidget.cpp
    CStatusWidget.cpp
    IDevice.cpp
    IDemodulator.cpp
    ISound.cpp
    ${qicompcr_SRCS}
)

if (WIN32)
    SET(qicompcr_SRCS ${qicompcr_SRCS} qicompcr.rc)
endif (WIN32)

if(WITH_PORTAUDIO)
    set(qicompcr_HDRS ${qicompcr_HDRS} CPortAudio.h)
endif(WITH_PORTAUDIO)


if(WITH_PULSEAUDIO)
    set(qicompcr_HDRS ${qicompcr_HDRS} CPulseSound.h)
endif(WITH_PULSEAUDIO)

if(WITH_ACARS)
    set(qicompcr_HDRS ${qicompcr_HDRS} CAcars.h)
endif(WITH_ACARS)


set(qicompcr_HDRS
    CAcarsGPL.h
    CApplication.h
    CCommand.h
    CDebugWindow.h
    CDemodulator.h
    CDevicePCR2500.h
    CFft.h
    CLcdWidget.h
    CMainWindow.h
    CMorse.h
    CPogsag.h
    CRemoteControl.h
    CSound.h
    CSpectrumWidget.h
    CStatusWidget.h
    version.h
    IDevice.h
    IDemodulator.h
    ISound.h
    ${qicompcr_HDRS}
)


SET(qicompcr_UIS
    CMainWindow.ui
)

SET(qicompcr_RCS
    resources.qrc
)


SET (QL_OPENGL_INCLUDE_DIR "")
SET (QL_OPENGL_LIBRARIES "")

add_definitions(-DDB_VERSION=8)
SET(qicompcr_HDRS ${qicompcr_HDRS}  )
SET(qicompcr_SRCS ${qicompcr_SRCS}  )
SET(qicompcr_UIS ${qicompcr_UIS} )

qt4_wrap_cpp(qicompcr_MOC_SRCS ${qicompcr_HDRS})
qt4_wrap_ui(qicompcr_UI_HDRS ${qicompcr_UIS})
qt4_add_resources(qicompcr_RC_SRCS ${qicompcr_RCS})

SET (ENABLE_TRANSLATION ON CACHE BOOL "enable translations")
#-D ENABLE_TRANSLATION:bool=OFF
IF(ENABLE_TRANSLATION)
    add_definitions(-DENABLE_TRANSLATION)

    file (GLOB TRANSLATIONS_FILES locale/*.ts)

    option (UPDATE_TRANSLATIONS "Update source translation translations/*.ts
    files (WARNING: make clean will delete the source .ts files! Danger!)")
    if (UPDATE_TRANSLATIONS)
       qt4_create_translation(qicompcr_QM_FILES
                              ${qicompcr_SRCS}
                              ${qicompcr_HDRS}
                              ${qicompcr_UIS}
                              ${TRANSLATIONS_FILES}
                          )
    else (UPDATE_TRANSLATIONS)
       qt4_add_translation(qicompcr_QM_FILES ${TRANSLATIONS_FILES})
    endif (UPDATE_TRANSLATIONS)

    if (UNIX AND NOT WIN32 AND NOT APPLE)
     install(
             FILES
                   ${qicompcr_QM_FILES}
             DESTINATION
                   ${DATA_INSTALL_PREFIX}/qicompcr/translations)
    endif (UNIX AND NOT WIN32 AND NOT APPLE)
    if (APPLE)
      add_custom_target(locversion.plist ALL
        COMMAND
            ${CMAKE_SOURCE_DIR}/MacOSX/mklocversion.sh ${MAC_BUNDLE_PATH} ${qicompcr_QM_FILES}
      )
    endif (APPLE)
ENDIF(ENABLE_TRANSLATION)

if(UNIX)
    add_definitions(-D_TTY_POSIX_ -Wall )
endif(UNIX)

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${QT_QTXML_INCLUDE_DIR}
  ${QT_QTSQL_INCLUDE_DIR}
  ${QT_QTOPENGL_INCLUDE_DIR}
  ${QT_QTNETWORK_INCLUDE_DIR}
  ${QT_QTDBUS_INCLUDE_DIR}
  ${QT_QTWEBKIT_INCLUDE_DIR}
  ${QT_QTSCRIPT_INCLUDE_DIR}
  ${QT_QTMULTIMEDIA_INCLUDE_DIR}
  ${QL_OPENGL_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/3rdparty/qwt
  ${CMAKE_SOURCE_DIR}/3rdparty/SerialPort
  ${CMAKE_SOURCE_DIR}/3rdparty/getopt
  ${CMAKE_SOURCE_DIR}/3rdparty/spuc
  ${CMAKE_SOURCE_DIR}/3rdparty/spuc/generic
  ${CMAKE_SOURCE_DIR}/3rdparty
  ${PROJ_INCLUDE_DIRS}
)

set(qicompcr_ALLINP
    ${qicompcr_SRCS}
    ${qicompcr_HDRS}
    ${qicompcr_UI_HDRS}
    ${qicompcr_RC_SRCS}
    ${qicompcr_MOC_SRCS}
    ${qicompcr_QM_FILES}
)

if (APPLE)
  set(MAC_EXTRA_RES
      ${CMAKE_SOURCE_DIR}/MacOSX/${MAC_ICON_FILE}
      ${CMAKE_SOURCE_DIR}/MacOSX/qicompcr-gdb.icns
      ${CMAKE_SOURCE_DIR}/MacOSX/qicompcr-gpx.icns
      ${CMAKE_SOURCE_DIR}/MacOSX/qicompcr-kml.icns
      ${CMAKE_SOURCE_DIR}/MacOSX/qicompcr-loc.icns
      ${CMAKE_SOURCE_DIR}/MacOSX/qicompcr-plt.icns
      ${CMAKE_SOURCE_DIR}/MacOSX/qicompcr-qlb.icns
      ${CMAKE_SOURCE_DIR}/MacOSX/qicompcr-rte.icns
      ${CMAKE_SOURCE_DIR}/MacOSX/qicompcr-tcx.icns
      ${CMAKE_SOURCE_DIR}/MacOSX/qicompcr-wpt.icns
  )
  set_source_files_properties(
    ${MAC_EXTRA_RES} ${qicompcr_QM_FILES}
  PROPERTIES
    MACOSX_PACKAGE_LOCATION Resources
  )
  set(MACOSX_BUNDLE_COPYRIGHT
    "Copyright © 2012 Fabrice Crohas (fcrohas@gmail.com)"
  )
  add_executable(${PROGNAME} MACOSX_BUNDLE ${qicompcr_ALLINP} ${MAC_EXTRA_RES})
  set_target_properties(${PROGNAME} PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/MacOSX/Info.plist.in
        LINK_FLAGS "-framework CoreFoundation"
  )

  add_custom_target(mac-deploy macdeployqt "${MAC_BUNDLE_PATH}"
    COMMAND ${CMAKE_SOURCE_DIR}/MacOSX/map2gcm-libs.sh "${MAC_BUNDLE_PATH}")
else(APPLE)
  add_executable(${PROGNAME} WIN32 ${qicompcr_ALLINP})
endif(APPLE)

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE )
endif(MSVC)

set(3RDPARTY ${3RDPARTY} qwt SerialPort getopt spuc)

if(WITH_PULSEAUDIO)
    set(3RDPARTY ${3RDPARTY} pulse-simple pulse)
endif(WITH_PULSEAUDIO)

if(WITH_ACARS)
    set(3RDPARTY ${3RDPARTY} acarsd)
endif(WITH_ACARS)

if(WITH_FFTW)
    set(3RDPARTY ${3RDPARTY} fftw3)
endif(WITH_FFTW)

if(WITH_PORTAUDIO)
    set(3RDPARTY ${3RDPARTY} portaudio)
endif(WITH_PORTAUDIO)

target_link_libraries(
      ${PROGNAME}
      ${GDAL_LIBRARIES}
      ${PROJ_LIBRARIES}
      ${QT_LIBRARIES}
      ${QT_QTNETWORK_LIBRARIES}
      ${QT_QTXML_LIBRARIES}
      ${QT_QTSQL_LIBRARIES}
      ${QT_QTDBUS_LIBRARIES}
      ${QT_QTMAIN_LIBRARIES}
      ${QT_QTWEBKIT_LIBRARIES}
      ${QT_PHONON_LIBRARIES}
      ${QT_QTSCRIPT_LIBRARIES}
      ${QT_QTMULTIMEDIA_LIBRARIES}
      ${QT_QTXMLPATTERNS_LIBRARIES}
      ${QL_OPENGL_LIBRARIES}
      ${3RDPARTY}
  )

install(
    TARGETS
      ${PROGNAME}
    DESTINATION
      ${BIN_INSTALL_DIR}
)
