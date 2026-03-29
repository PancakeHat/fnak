cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_COLOR_DIAGNOSTICS true)

project(app)

# Add ImGui Files to source
file(GLOB IMGUISOURCES
    ${CMAKE_SOURCE_DIR}/include/rlimgui/*.cpp
)


# Set up executable
if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "-static -static-libstdc++ -static-libgcc")
    add_definitions(-D_THREAD_SAFE)
    
    # DEBUG BINARY
    # add_executable(fnak src/main.cpp fnak.rc ${IMGUISOURCES})

    # RELEASE BINARY
    add_executable(fnak WIN32 src/main.cpp fnak.rc ${IMGUISOURCES})

else()
    add_executable(fnak src/main.cpp ${IMGUISOURCES})
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include/)
include_directories(${CMAKE_SOURCE_DIR}/include/rlimgui)

# Linux setup
if(LINUX)
    target_compile_options(fnak PRIVATE
        "-std=c++20"
    )

    add_library(raylib STATIC IMPORTED)
    set_target_properties(raylib PROPERTIES
        IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/lib/linux/libraylib.a"
    )

    target_link_libraries(fnak PRIVATE raylib GL m pthread dl rt X11)
    target_link_options(fnak PRIVATE -static-libstdc++ -static-libgcc)
endif()

# Link with MacOS libraries
if(APPLE)
    target_link_libraries(fnak "-framework CoreVideo")
    target_link_libraries(fnak "-framework IOKit")
    target_link_libraries(fnak "-framework Cocoa")
    target_link_libraries(fnak "-framework GLUT")
    target_link_libraries(fnak "-framework OpenGL")

    target_link_libraries(fnak ${CMAKE_SOURCE_DIR}/lib/macos/libraylib.a)
endif()

# Link with Windows libraries
if(WIN32)
    target_link_libraries(fnak ${CMAKE_SOURCE_DIR}/lib/win/libraylib.a winmm gdi32)
endif()