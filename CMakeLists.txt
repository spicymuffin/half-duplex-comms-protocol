cmake_minimum_required(VERSION 3.12)
project(half-duplex-comms-protocol)

# Add your source files here
set(SOURCES
    main.cpp
    CommunicationSystem.cpp
)

# Add any additional include directories here
include_directories(include)

# Add any additional libraries here
# find_package(YourLibrary REQUIRED)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link any additional libraries here
# target_link_libraries(${PROJECT_NAME} YourLibrary::YourLibrary)