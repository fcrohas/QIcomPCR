project(getopt)

# Required cmake version
cmake_minimum_required(VERSION 2.6.0)


set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

find_package(Qt5 REQUIRED)

include(${QT_USE_FILE})

set(SRCS
    CGetOpt.cpp
)

set(HDRS
    CGetOpt.h
)

set(UIS

)

set(RCS


)


qt5_wrap_cpp(MOC_SRCS ${HDRS})
qt5_wrap_ui(UI_HDRS ${UIS})
qt5_add_resources(RC_SRCS ${RCS})



include_directories(
	${QT_QTMULTIMEDIA_INCLUDE_DIR}
)

link_directories(
    ${CMAKE_BINARY_DIR}/lib
)

add_library(getopt
  STATIC
  ${SRCS}
  ${HDRS}
  ${UI_HDRS}
  ${RC_SRCS}
  ${MOC_SRCS}
  ${QM_FILES}
)

target_link_libraries( getopt
      ${QT_LIBRARIES}
)
