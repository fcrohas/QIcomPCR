# define system dependent compiler flags
# only build 32-bit binaries

set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS OFF)
set_property(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB_PATHS ON)

include(CheckCXXCompilerFlag)

if(UNIX AND NOT WIN32)
    add_definitions(-m32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/usr/lib -m32")
    set(CMAKE_SHARED_LIBRARY_C_FLAGS "${CMAKE_SHARED_LIBRARY_C_FLAGS} -L/usr/lib -m32")
    set(CMAKE_SHARED_LIBRARY_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_CXX_FLAGS} -L/usr/lib -m32")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
    set(COMPILE_CFLAGS "${COMPILE_CFLAGS} -m32")
    set(COMPILE_CXXFLAGS "${COMPILE_CXXFLAGS} -m32")
    set(LINK_FLAGS "${LINK_FLAGS} -L/usr/lib -m32")
    set(CMAKE_PREFIX_PATH "/usr/lib")
endif(UNIX AND NOT WIN32)

# with -fPIC
if(UNIX AND NOT WIN32)
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    check_cxx_compiler_flag("-fPIC" WITH_FPIC)
    if(WITH_FPIC)
      ADD_DEFINITIONS(-fPIC)
    endif(WITH_FPIC)
  endif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
endif(UNIX AND NOT WIN32)
