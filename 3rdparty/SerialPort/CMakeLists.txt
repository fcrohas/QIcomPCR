project(SerialPort)

# Required cmake version
cmake_minimum_required(VERSION 2.6.0)


set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

find_package(Qt4 REQUIRED)

include(${QT_USE_FILE})

set(SRCS
    ManageSerialPort.cpp
#    posix_qextserialport.cpp
    qextserialbase.cpp
    qextserialport.cpp
#    win_qextserialport.cpp
)

set(HDRS
    ManageSerialPort.h
#    posix_qextserialport.h
    qextserialbase.h
    qextserialport.h
#    win_qextserialport.h
)

set(UIS

)

set(RCS


)

if(WIN32)
    set(SRCS ${SRCS} win_qextserialport.cpp)
    set(HDRS ${HDRS} win_qextserialport.h)
else (WIN32)
    set(SRCS ${SRCS} posix_qextserialport.cpp)
    set(HDRS ${HDRS} posix_qextserialport.h)
    add_definitions(-D_TTY_POSIX_)
endif(WIN32)


qt4_wrap_cpp(MOC_SRCS ${HDRS})
qt4_wrap_ui(UI_HDRS ${UIS})
qt4_add_resources(RC_SRCS ${RCS})



include_directories(

)

link_directories(
    ${CMAKE_BINARY_DIR}/lib
)

add_library(SerialPort
  ${SRCS}
  ${HDRS}
  ${UI_HDRS}
  ${RC_SRCS}
  ${MOC_SRCS}
  ${QM_FILES}
)

target_link_libraries( SerialPort
      ${QT_LIBRARIES}
)
