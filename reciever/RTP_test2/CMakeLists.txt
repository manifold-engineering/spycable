cmake_minimum_required(VERSION 3.16)

project(RTP_test2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(RTP_test2 main.cpp
    audioringbuffer.h audioringbuffer.cpp)

include(GNUInstallDirs)
install(TARGETS RTP_test2
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
