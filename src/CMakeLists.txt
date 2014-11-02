project(qicompcr-binary)

option(WITH_OPENGL "Build OPENGL" ON)
option(WITH_ACARS "Build with licensed ACARS Library" OFF)
option(WITH_FFTW "Build with Fast Fourrier Transform Library" ON)
option(WITH_PULSEAUDIO "Build with PulseAudio support" OFF)
option(WITH_PORTAUDIO "Build with PortAudio support" OFF)
option(WITH_SNDFILE "Build with SndFile for loading sound file support" ON)
option(WITH_SAMPLERATE "Build with resample capabilities support" ON)
option(WITH_SPEEX "Build with Speex library for streaming sound support" ON)
option(WITH_GUI "Build with Qt gui library for graphical interface support" ON)
option(WITH_ICOMPCR "Build with ICOM PCR driver" OFF)
option(WITH_RTLSDR "Build with RTLSDR driver" ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)

# Use the Widgets module from Qt 5.
#qt5_use_modules(qicompcr-binary Widgets Network)

if(WITH_OPENGL)
    find_package(OpenGL REQUIRED)
endif(WITH_OPENGL)

#include(${QT_USE_FILE})
#set(QT_LIBRARY_DIR "/usr/lib")

if(WIN32)
    add_definitions(-D_USE_MATH_DEFINES)
endif(WIN32)

if(UNIX)
    set(qicompcr_SRCS ${qicompcr_SRCS} ${CMAKE_SOURCE_DIR}/src/decoder/ctype.c)
endif(UNIX)

if(WITH_SAMPLERATE)
    add_definitions(-DWITH_SAMPLERATE)
endif(WITH_SAMPLERATE)

if(WITH_PULSEAUDIO)
    set(qicompcr_SRCS ${qicompcr_SRCS} ${CMAKE_SOURCE_DIR}/src/backend/sound/CPulseSound.cpp)
    set(qicompcr_HDRS ${qicompcr_HDRS} ${CMAKE_SOURCE_DIR}/src/backend/sound/CPulseSound.h)
    add_definitions(-DWITH_PULSEAUDIO)
endif(WITH_PULSEAUDIO)

if(WITH_PORTAUDIO)
    set(qicompcr_SRCS ${qicompcr_SRCS} ${CMAKE_SOURCE_DIR}/src/backend/sound/CPortAudio.cpp )
    set(qicompcr_HDRS ${qicompcr_HDRS} ${CMAKE_SOURCE_DIR}/src/backend/sound/CPortAudio.h)
    add_definitions(-DWITH_PORTAUDIO)
endif(WITH_PORTAUDIO)

if(WITH_SPEEX)
    add_definitions(-DWITH_SPEEX)
endif(WITH_SPEEX)

if(WITH_ACARS)
    set(qicompcr_SRCS ${qicompcr_SRCS} ${CMAKE_SOURCE_DIR}/src/decoder/CAcars.cpp)
    add_definitions(-DWITH_ACARS)
endif(WITH_ACARS)

if(WITH_FFTW)
     add_definitions(-DFFTW)
endif(WITH_FFTW)

if(WITH_SNDFILE)
    set(qicompcr_SRCS ${qicompcr_SRCS} ${CMAKE_SOURCE_DIR}/src/backend/sound/CSoundFile.cpp)
    set(qicompcr_HDRS ${qicompcr_HDRS} ${CMAKE_SOURCE_DIR}/src/backend/sound/CSoundFile.h)
    add_definitions(-DWITH_SNDFILE)
endif(WITH_SNDFILE)

if(WITH_ACARS)
    set(qicompcr_HDRS ${qicompcr_HDRS} ${CMAKE_SOURCE_DIR}/src/decoder/CAcars.h)
endif(WITH_ACARS)

if(WITH_ICOMPCR)
    set(qicompcr_HDRS ${qicompcr_HDRS} ${CMAKE_SOURCE_DIR}/src/backend/devices/pcr2500/CCommand.h ${CMAKE_SOURCE_DIR}/src/backend/devices/pcr2500/CDevicePCR2500.h)
    set(qicompcr_SRCS ${qicompcr_SRCS} ${CMAKE_SOURCE_DIR}/src/backend/devices/pcr2500/CCommand.cpp ${CMAKE_SOURCE_DIR}/src/backend/devices/pcr2500/CDevicePCR2500.cpp)
    add_definitions(-DWITH_ICOMPCR)
endif(WITH_ICOMPCR)

if(WITH_RTLSDR)
    set(qicompcr_HDRS ${qicompcr_HDRS} ${CMAKE_SOURCE_DIR}/src/backend/devices/rtlsdr/CCommand.h ${CMAKE_SOURCE_DIR}/src/backend/devices/rtlsdr/CRtlSdr.h)
    set(qicompcr_SRCS ${qicompcr_SRCS} ${CMAKE_SOURCE_DIR}/src/backend/devices/rtlsdr/CCommand.cpp ${CMAKE_SOURCE_DIR}/src/backend/devices/rtlsdr/CRtlSdr.cpp)
    add_definitions(-DWITH_RTLSDR)
endif(WITH_RTLSDR)

if(WITH_GUI)
    set(qicompcr_SRCS ${qicompcr_SRCS}
        ${CMAKE_SOURCE_DIR}/src/gui/widget/AdvPlotZoomer.cpp
        ${CMAKE_SOURCE_DIR}/src/gui/widget/CBandScope.cpp
        ${CMAKE_SOURCE_DIR}/src/gui/widget/CDebugWindow.cpp
        ${CMAKE_SOURCE_DIR}/src/gui/widget/CLcdWidget.cpp
        ${CMAKE_SOURCE_DIR}/src/gui/widget/CSpectrumWidget.cpp
        ${CMAKE_SOURCE_DIR}/src/gui/widget/CStatusWidget.cpp
        ${CMAKE_SOURCE_DIR}/src/gui/widget/CDisplay.cpp
        ${CMAKE_SOURCE_DIR}/src/gui/widget/CwPicker.cpp
        ${CMAKE_SOURCE_DIR}/src/gui/widget/RttyPicker.cpp
        ${CMAKE_SOURCE_DIR}/src/gui/widget/ThresholdPicker.cpp
        ${CMAKE_SOURCE_DIR}/src/gui/CMainWindow.cpp
        ${CMAKE_SOURCE_DIR}/src/gui/CDlgSettings.cpp
    )
    set(qicompcr_HDRS ${qicompcr_HDRS}
        ${CMAKE_SOURCE_DIR}/src/gui/CMainWindow.h
        ${CMAKE_SOURCE_DIR}/src/gui/CDlgSettings.h
        ${CMAKE_SOURCE_DIR}/src/gui/widget/AdvPlotZoomer.h
        ${CMAKE_SOURCE_DIR}/src/gui/widget/CBandScope.h
        ${CMAKE_SOURCE_DIR}/src/gui/widget/CLcdWidget.h
        ${CMAKE_SOURCE_DIR}/src/gui/widget/CSpectrumWidget.h
        ${CMAKE_SOURCE_DIR}/src/gui/widget/CStatusWidget.h
        ${CMAKE_SOURCE_DIR}/src/gui/widget/CDebugWindow.h
        ${CMAKE_SOURCE_DIR}/src/gui/widget/CDisplay.h
        ${CMAKE_SOURCE_DIR}/src/gui/widget/CwPicker.h
        ${CMAKE_SOURCE_DIR}/src/gui/widget/RttyPicker.h
        ${CMAKE_SOURCE_DIR}/src/gui/widget/ThresholdPicker.h
    )
    add_definitions(-DWITH_GUI)
endif(WITH_GUI)

set(qicompcr_SRCS
    main.cpp
    CApplication.cpp
    ${CMAKE_SOURCE_DIR}/3rdparty/portaudio/pa_ringbuffer.c
    ${CMAKE_SOURCE_DIR}/src/decoder/CAcarsGPL.cpp
    ${CMAKE_SOURCE_DIR}/src/decoder/CRtty.cpp
    ${CMAKE_SOURCE_DIR}/src/decoder/CMorse.cpp
    ${CMAKE_SOURCE_DIR}/src/decoder/CPogsag.cpp
    ${CMAKE_SOURCE_DIR}/src/backend/demodulator/CDemodulator.cpp
    ${CMAKE_SOURCE_DIR}/src/backend/demodulator/CFft.cpp
    ${CMAKE_SOURCE_DIR}/src/backend/sound/CSoundStream.cpp
    ${CMAKE_SOURCE_DIR}/src/backend/CRemoteControl.cpp
    ${CMAKE_SOURCE_DIR}/src/backend/sound/CSound.cpp
    ${CMAKE_SOURCE_DIR}/src/backend/IDevice.cpp
    ${CMAKE_SOURCE_DIR}/src/backend/IDemodulator.cpp
    ${CMAKE_SOURCE_DIR}/src/backend/ISound.cpp
    ${CMAKE_SOURCE_DIR}/src/backend/CSettings.cpp
    ${CMAKE_SOURCE_DIR}/src/backend/CFir.cpp
    ${CMAKE_SOURCE_DIR}/src/backend/CIir.cpp
    ${CMAKE_SOURCE_DIR}/src/backend/CWindowFunc.cpp
    ${CMAKE_SOURCE_DIR}/src/backend/CBackend.cpp
    ${qicompcr_SRCS}
)

if (WIN32)
    SET(qicompcr_SRCS ${qicompcr_SRCS} qicompcr.rc)
endif (WIN32)

set(qicompcr_HDRS
    ${CMAKE_SOURCE_DIR}/src/decoder/CAcarsGPL.h
    ${CMAKE_SOURCE_DIR}/src/decoder/CRtty.h
    ${CMAKE_SOURCE_DIR}/src/decoder/CMorse.h
    ${CMAKE_SOURCE_DIR}/src/decoder/CPogsag.h
    ${CMAKE_SOURCE_DIR}/src/backend/sound/CSoundStream.h
    ${CMAKE_SOURCE_DIR}/src/backend/CRemoteControl.h
    ${CMAKE_SOURCE_DIR}/src/backend/demodulator/CDemodulator.h
    ${CMAKE_SOURCE_DIR}/src/backend/demodulator/CFft.h
    ${CMAKE_SOURCE_DIR}/src/backend/sound/CSound.h
    ${CMAKE_SOURCE_DIR}/src/backend/IDevice.h
    ${CMAKE_SOURCE_DIR}/src/backend/IDemodulator.h
    ${CMAKE_SOURCE_DIR}/src/backend/ISound.h
    ${CMAKE_SOURCE_DIR}/src/backend/CSettings.h
    ${CMAKE_SOURCE_DIR}/src/backend/CFir.h
    ${CMAKE_SOURCE_DIR}/src/backend/CIir.h
    ${CMAKE_SOURCE_DIR}/src/backend/CWindowFunc.h
    ${CMAKE_SOURCE_DIR}/src/backend/consts.h
    ${CMAKE_SOURCE_DIR}/src/backend/CBackend.h
    CApplication.h
    version.h
    ${qicompcr_HDRS}
)


SET(qicompcr_UIS
    ${CMAKE_SOURCE_DIR}/src/gui/CMainWindow.ui
    ${CMAKE_SOURCE_DIR}/src/gui/CDlgSettings.ui
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

qt5_wrap_cpp(qicompcr_MOC_SRCS ${qicompcr_HDRS})
qt5_wrap_ui(qicompcr_UI_HDRS ${qicompcr_UIS})
qt5_add_resources(qicompcr_RC_SRCS ${qicompcr_RCS})

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
       #qt5_add_translation(qicompcr_QM_FILES ${TRANSLATIONS_FILES})
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
  ${CMAKE_SOURCE_DIR}/src/backend
  ${CMAKE_SOURCE_DIR}/src/backend/sound
  ${CMAKE_SOURCE_DIR}/src/backend/demodulator
  ${CMAKE_SOURCE_DIR}/src/backend/devices/rtlsdr
  ${CMAKE_SOURCE_DIR}/src/gui
  ${CMAKE_SOURCE_DIR}/src/gui/widget
  ${CMAKE_SOURCE_DIR}/src/decoder
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
  ${Qt5Widgets_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/3rdparty/qwt
  ${CMAKE_SOURCE_DIR}/3rdparty/SerialPort
  ${CMAKE_SOURCE_DIR}/3rdparty/getopt
  ${CMAKE_SOURCE_DIR}/3rdparty/spuc
  ${CMAKE_SOURCE_DIR}/3rdparty/spuc/generic
  ${CMAKE_SOURCE_DIR}/3rdparty/dspfilters
  ${CMAKE_SOURCE_DIR}/3rdparty
  ${CMAKE_SOURCE_DIR}/3rdparty/portaudio
  ${CMAKE_SOURCE_DIR}/3rdparty/libsndfile/include
  ${CMAKE_SOURCE_DIR}/3rdparty/libsamplerate/include
  ${CMAKE_SOURCE_DIR}/3rdparty/fftw3
  ${CMAKE_SOURCE_DIR}/3rdparty/speex/include
  ${CMAKE_SOURCE_DIR}/3rdparty/acarsd
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
  qt5_use_modules(${PROGNAME} Widgets Network Svg)
endif(APPLE)

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE )
endif(MSVC)

set(3RDPARTY ${3RDPARTY} qwt SerialPort getopt ) #spuc DspFilters

if(WITH_PULSEAUDIO)
    set(3RDPARTY ${3RDPARTY} pulse-simple pulse)
endif(WITH_PULSEAUDIO)

if(WITH_ACARS)
if(UNIX)
    set(3RDPARTY ${3RDPARTY} acarsd)
endif(UNIX)
if(WIN32)
    set(3RDPARTY ${3RDPARTY} ${CMAKE_SOURCE_DIR}/3rdparty/acarsd/libacarsd.lib)
endif(WIN32)
endif(WITH_ACARS)

if(WITH_FFTW)
if(UNIX)
    set(3RDPARTY ${3RDPARTY} fftw3)
endif(UNIX)
if(WIN32)
    set(3RDPARTY ${3RDPARTY} ${CMAKE_SOURCE_DIR}/3rdparty/fftw3/libfftw3-3.lib)
endif(WIN32)
endif(WITH_FFTW)

if(WITH_PORTAUDIO)
    set(3RDPARTY ${3RDPARTY} portaudio)
endif(WITH_PORTAUDIO)

if(WITH_SNDFILE)
if(UNIX)
    set(3RDPARTY ${3RDPARTY} sndfile)
endif(UNIX)
if(WIN32)
    set(3RDPARTY ${3RDPARTY} ${CMAKE_SOURCE_DIR}/3rdparty/libsndfile/lib/libsndfile-1.lib)
endif(WIN32)
endif(WITH_SNDFILE)

if(WITH_SAMPLERATE)
if(UNIX)
    set(3RDPARTY ${3RDPARTY} samplerate)
endif(UNIX)
if(WIN32)
    set(3RDPARTY ${3RDPARTY} ${CMAKE_SOURCE_DIR}/3rdparty/libsamplerate/lib/libsamplerate-0.lib)
endif(WIN32)
endif(WITH_SAMPLERATE)

if(WITH_SPEEX)
if(UNIX)
    set(3RDPARTY ${3RDPARTY} speex speexdsp)
endif(UNIX)
if(WIN32)
        set(3RDPARTY ${3RDPARTY} ${CMAKE_SOURCE_DIR}/3rdparty/speex/lib/libspeex.lib ${CMAKE_SOURCE_DIR}/3rdparty/speex/lib/libspeexdsp.lib)
endif(WIN32)
endif(WITH_SPEEX)


target_link_libraries(
      ${PROGNAME}
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
      ${Qt5_LIBRARIES}
      ${Qt5Widgets_LIBRARIES}
      ${3RDPARTY}
      ${Qt5Core_QTMAIN_LIBRARIES}
  )

install(
    TARGETS
      ${PROGNAME}
    DESTINATION
      ${BIN_INSTALL_DIR}
)
