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
  custom_stream_types.h
  rtpserver.h rtpserver.cpp
  rtpstreamchannel.h rtpstreamchannel.cpp
  config.h
  server_helper_functions.h
  deinterlacedbuffer.h deinterlacedbuffer.cpp
  preliminary_test.h
)
#target_link_libraries(RTP_test3 Qt${QT_VERSION_MAJOR}::Core)



# Configure PortAudio options
if(WIN32)
    # Windows-specific PortAudio options
    set(PA_USE_WASAPI ON CACHE BOOL "Enable WASAPI support")
    set(PA_USE_WDMKS ON CACHE BOOL "Enable WDMKS support")
    set(PA_USE_ASIO OFF CACHE BOOL "Enable ASIO support") # DISABLED by default
    set(PA_USE_DS ON CACHE BOOL "Enable DirectSound support")
    set(PA_USE_WMME ON CACHE BOOL "Enable MME support")
else()
    # Linux-specific PortAudio options
    set(PA_USE_ALSA ON CACHE BOOL "Enable ALSA support")
    set(PA_USE_JACK ON CACHE BOOL "Enable JACK support")
endif()


# Add PortAudio subdirectory - use your local copy in "portaudio" folder
# Let CMake know that portaudio should be treated as a C project
enable_language(C)
add_subdirectory(portaudio EXCLUDE_FROM_ALL)


# Check which target name PortAudio is using
# Both "portaudio_static" and "portaudio" are common target names
if(TARGET portaudio_static)
    target_link_libraries(RTP_test3 PRIVATE portaudio_static)
elseif(TARGET portaudio)
    target_link_libraries(RTP_test3 PRIVATE portaudio)
else()
    message(FATAL_ERROR "PortAudio target not found")
endif()

# Ensure we have the PortAudio include directory
target_include_directories(RTP_test3 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/portaudio/include
)

# Installation config
include(GNUInstallDirs)
install(TARGETS RTP_test3
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)



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
