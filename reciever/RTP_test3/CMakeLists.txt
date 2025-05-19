cmake_minimum_required(VERSION 3.16)

project(RTP_test3 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(RTP_test3
  main.cpp
  audioringbuffer.h audioringbuffer.cpp
)
#target_link_libraries(RTP_test3 Qt${QT_VERSION_MAJOR}::Core)

# Platform-specific networking libraries
if(WIN32)
    # Windows-specific settings
    target_link_libraries(RTP_test3 PRIVATE wsock32 ws2_32)
    target_compile_definitions(RTP_test3 PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
else()
    # Linux/Unix-specific settings (nothing additional needed for BSD sockets)
    # BSD sockets are part of the standard C library on Unix-like systems
endif()

include(GNUInstallDirs)
install(TARGETS RTP_test3
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
