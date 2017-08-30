CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

project(TemperatureToConductance)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

find_package(Boost COMPONENTS system filesystem REQUIRED)
#...


set(SOURCE_FILES
  main.cpp
  ImageConverter.cpp
  ImageConverter.hpp
)

add_executable(TemperatureToConductance ${SOURCE_FILES})

target_link_libraries(TemperatureToConductance
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
)
