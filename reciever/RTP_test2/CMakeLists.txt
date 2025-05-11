cmake_minimum_required(VERSION 3.16)
project(RTP_test2 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set source files
set(SOURCE_FILES
    main.cpp
    audioringbuffer.h
    audioringbuffer.cpp
    test_ringbuffer.h
    rtpserver.h
    rtpserver.cpp
    rtpstreamchannel.h
    rtpstreamchannel.cpp
    config.h
)

# Create executable
add_executable(RTP_test2 ${SOURCE_FILES})

# Platform-specific networking libraries
if(WIN32)
    # Windows-specific settings
    target_link_libraries(RTP_test2 PRIVATE wsock32 ws2_32)
    target_compile_definitions(RTP_test2 PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
else()
    # Linux/Unix-specific settings (nothing additional needed for BSD sockets)
    # BSD sockets are part of the standard C library on Unix-like systems
endif()

# Installation config
include(GNUInstallDirs)
install(TARGETS RTP_test2
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
