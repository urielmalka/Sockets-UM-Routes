cmake_minimum_required(VERSION 3.10)
project(Sockum)

set(CMAKE_CXX_STANDARD 17)

# Define paths
set(UCOLOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/UColor_C)
set(UCOLOR_HEADER ${UCOLOR_DIR}/include/color.h)

# Step 1: Download and extract UColor_C if not already present
if(NOT EXISTS ${UCOLOR_HEADER})
    message(STATUS "Downloading UColor_C...")

    file(DOWNLOAD
        https://github.com/urielmalka/UColor_C/archive/refs/heads/main.zip
        ${CMAKE_CURRENT_BINARY_DIR}/UColor_C.zip
        SHOW_PROGRESS
        STATUS dl_status
    )
    list(GET dl_status 0 dl_code)
    if(NOT dl_code EQUAL 0)
        message(FATAL_ERROR "Download failed with code: ${dl_code}")
    endif()

    # Extract zip
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xfz ${CMAKE_CURRENT_BINARY_DIR}/UColor_C.zip
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/external
    )

    # Remove existing if necessary
    if(EXISTS ${UCOLOR_DIR})
        file(REMOVE_RECURSE ${UCOLOR_DIR})
    endif()

    # Move extracted folder into expected path
    file(RENAME
        ${CMAKE_CURRENT_SOURCE_DIR}/external/UColor_C-main/UColor_C
        ${UCOLOR_DIR}
    )

    # Clean up
    file(REMOVE_RECURSE ${CMAKE_CURRENT_SOURCE_DIR}/external/UColor_C-main)
endif()

# Step 2: Gather source files
file(GLOB SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/external/UColor_C/src/color.c
)

# Step 3: Define the library
add_library(Sockum STATIC ${SRC_FILES})

# Step 4: Include directories
target_include_directories(Sockum PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/utils
    ${UCOLOR_DIR}/include
)

message(STATUS "Configured Sockum project with UColor_C.")
