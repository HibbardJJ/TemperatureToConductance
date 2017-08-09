#include <iostream>
#include <vector>

#include "ImageConverter.hpp"

int main(void) {
  std::string baseDirectory = "/Users/katiesweet/Desktop/Patchy/";

  ImageConverter temperatureToConductance(baseDirectory);
  temperatureToConductance.getUserInputs();
  temperatureToConductance.loadNecessaryFiles();
  temperatureToConductance.calculateConductanceMaps();
  // temperatureToConductance.summarizeSelectedPixels();
  return 0;
}

// Config File
// Image # | Average Air Temperature | Wa lower
