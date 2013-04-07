project(DspFilters)

# Required cmake version
cmake_minimum_required(VERSION 2.6.0)


set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

include_directories (${HELLO_SOURCE_DIR}/include) 

set(SRCS
    Bessel.cpp
    Butterworth.cpp
    Biquad.cpp
    Cascade.cpp
    ChebyshevI.cpp
    ChebyshevII.cpp
    Custom.cpp
    Design.cpp
    Documentation.cpp
    Elliptic.cpp
    Filter.cpp
    Legendre.cpp
    Param.cpp
    PoleFilter.cpp
    RBJ.cpp
    RootFinder.cpp
    State.cpp
)

set(HDRS
    Bessel.h
    Butterworth.h
    Biquad.h
    Cascade.h
    ChebyshevI.h
    ChebyshevII.h
    Common.h
    Custom.h
    Design.h
    Dsp.h
    Elliptic.h
    Filter.h
    Layout.h
    Legendre.h
    Params.h
    MathSupplement.h    
    PoleFilter.h
    RBJ.h
    RootFinder.h
    State.h
    SmoothedFilter.h
    Types.h
    Utilities.h
)

include_directories(

)

link_directories(
    ${CMAKE_BINARY_DIR}/lib
)

add_library(DspFilters
  ${SRCS}
  ${HDRS}
)

target_link_libraries( DspFilters
)
