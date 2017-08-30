#include <iostream>
#include <vector>

#include "ImageConverter.hpp"

int main(void) {
  std::string baseDirectory = "/Users/katiesweet/Desktop/Patchy/";

  ImageConverter temperatureToConductance(baseDirectory);
  // temperatureToConductance.chooseProgramTypeAndExecute();
  return 0;
}

// Config File
// Image # | Average Air Temperature | Wa lower

// TODO:
// 1. Add option for creating KMatrix
// 2. Add option for reading data and printing selected pixels
