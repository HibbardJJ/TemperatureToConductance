CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

project(TemperatureToConductance)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES
  main.cpp
  #FileSystemAccess.cpp
  #FileSystemAccess.hpp
  ImageConverter.cpp
  ImageConverter.hpp
)

# add_library(ProjectLibrary
#   main.cpp
#   #FileSystemAccess.cpp
#   #FileSystemAccess.hpp
#   ImageConverter.cpp
#   ImageConverter.hpp
# )

# target_link_libraries(TemperatureToConductance ProjectLibrary)

add_executable(TemperatureToConductance ${SOURCE_FILES})
