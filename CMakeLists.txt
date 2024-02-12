
cmake_minimum_required(VERSION 3.15)
project(CrowSample)
project(CrowLibrary)

# Specify here which libraries do you want to use
set(INCLUDE_PATHS boost_1_81_0 crow/include)

# Specify what executables you want to generate
add_executable(CrowSample main.cpp)
add_executable(CrowLibrary crowLibraryServer.cpp)

# [!!!] Necessary addition; pthread for some reason isn't linked automatically
target_link_libraries(CrowSample pthread)
target_link_libraries(CrowLibrary pthread)

target_include_directories(CrowSample PUBLIC ${INCLUDE_PATHS})
target_include_directories(CrowLibrary PUBLIC ${INCLUDE_PATHS})
