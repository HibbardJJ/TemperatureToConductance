#include "ImageConverter.hpp"
#include <dirent.h>
#include <fstream>
#include <iostream>

/* Constructor for the Image Converter. Takes a string that holds the base
 * working directory for all of the project. */
ImageConverter::ImageConverter(std::string baseDirectory)
    : baseWorkingDirectory(baseDirectory) {}

///////////////////////// INPUT FUNCTIONS //////////////////////////////////////

/* Function that gets all the necessary inputs from the user. */
void ImageConverter::getUserInputs() {
  getDate();
  getKMatrix();
  getRawTemperatureDir();
  getProgramInputFile();
  getBaseDir();
  getRValue();
}

/* Gets the date from the user. Uses this to try and guess where the rest of the
 * data is, as well as write files with date in the title */
void ImageConverter::getDate() {
  std::cout << "Please enter the date of the data used (YYYY-MM-DD)."
            << std::endl;
  std::getline(std::cin, date);
}

/* Asks for the appropriate location of the KMatrix. This tries to appropriately
 * guess the location of the directory based on the base directory of the
 * project. */
void ImageConverter::getKMatrix() {
  std::string usrOutputMessage = "K Matrix";
  kMatrixLocation = baseWorkingDirectory + "KMatrix/KMatrix.csv";

  if (!checkCorrectLocation(usrOutputMessage, kMatrixLocation)) {
    kMatrixLocation = getCorrectLocation(usrOutputMessage);
  }
}

/* Guesses and asks to confirm the appropriate location of the directory that
 * holds the raw temperatures from the thermal imaging camera. Each file that
 * should be averaged together should have the same suffix (i.e. imageName_1)."
 */
void ImageConverter::getRawTemperatureDir() {
  std::string usrOutputMessage = "temperature images directory";
  rawTemperatureDirectory =
      baseWorkingDirectory + "Data/" + date + "/TempImages/";
  if (!checkCorrectLocation(usrOutputMessage, rawTemperatureDirectory)) {
    rawTemperatureDirectory = getCorrectLocation(usrOutputMessage);
  }
}

/* Guesses and asks to confirm the appropriate location of the file that holds
 * some necessary input for the program. The program expects the following
 * columns:
 *    Image Number (imageName_#) | Avg Air Temperature | Average W_a (xout)
 */
void ImageConverter::getProgramInputFile() {
  std::string usrOutputMessage = "program input file";
  programInputFile =
      baseWorkingDirectory + "Data/" + date + "/DataExtraction.csv";
  if (!checkCorrectLocation(usrOutputMessage, programInputFile)) {
    programInputFile = getCorrectLocation(usrOutputMessage);
  }
}

/* Asks for the appropriate location to save the files the program produces". */
void ImageConverter::getBaseDir() {
  std::string usrOutputMessage =
      "directory you wish to save the program's output files to";
  baseSaveDirectory = baseWorkingDirectory + "Data/" + date + "/";
  if (!checkCorrectLocation(usrOutputMessage, baseSaveDirectory)) {
    baseSaveDirectory = getCorrectLocation(usrOutputMessage);
  }
}

/* Asks for the RValue to use in the program. */
void ImageConverter::getRValue() {
  std::string rVal;
  std::cout << "Please enter the desired R Value." << std::endl;
  std::getline(std::cin, rVal);
  rValue = std::stoi(rVal);
}

/* Helper function that checks whether the guessed location for a certain file
 * or directory is correct. If it is correct, the program returns true.
 * Otherwise it returns false. */
bool ImageConverter::checkCorrectLocation(std::string outputMessage,
                                          std::string assumedLocation) {
  std::string correctLocation;
  std::cout << "Is the location of the " << outputMessage << " you wish to use \
  \'" << assumedLocation
            << "\'? [Enter y/n]" << std::endl;
  std::getline(std::cin, correctLocation);

  if (correctLocation[0] == 'y' || correctLocation[0] == 'Y') {
    return true;
  } else if (correctLocation[0] == 'n' || correctLocation[0] == 'N') {
    return false;
  } else {
    std::cout << "INVALID RESPONSE. Please try again." << std::endl;
    return checkCorrectLocation(outputMessage, assumedLocation);
  }
}

/* Helper function to get the correct location of a file or directory if the
 * user specifies that the assumed one is incorrect. */
std::string ImageConverter::getCorrectLocation(std::string outputMessage) {
  std::string newLocation;
  std::cout << "Please enter the full path to the " << outputMessage
            << " you wish to use. " << std::endl;
  std::getline(std::cin, newLocation);
  return newLocation;
}

///////////////////////// LOADING DATA FUNCTIONS //////////////////////////////

void ImageConverter::loadNecessaryFiles() {
  std::cout << "Loading necessary files. This could take a while..."
            << std::endl;
  loadKMatrix();
  loadDataFile();
  loadTemperatureData();
}

void ImageConverter::loadKMatrix() {
  kMatrix = loadImageFromFile(kMatrixLocation);
  // for (auto &&row : kMatrix) {
  //   for (auto &&entry : row) {
  //     std::cout << entry << " ";
  //   }
  //   std::cout << std::endl << std::endl << std::endl;
  // }
  // std::cout << "Completed loading KMatrix" << std::endl;
}

void ImageConverter::loadDataFile() {
  std::ifstream inputFile;

  std::cout << "Loading file: " << programInputFile << std::endl;
  inputFile.open(programInputFile);

  std::string inputLine;
  while (!inputFile.eof()) {
    std::getline(inputFile, inputLine);
    if (!inputLine.empty()) {
      std::istringstream rowToParse(inputLine);
      parseInputFileLine(rowToParse);
    }
  }

  inputFile.close();

  // std::cout << "Completed loading program's input file" << std::endl;
}

void ImageConverter::parseInputFileLine(std::istringstream &rowToParse) {
  std::string data;

  // Read image number
  std::getline(rowToParse, data, ',');
  int imageNumber = std::stoi(data);
  // std::cout << imageNumber;

  // Read air temperature;
  std::getline(rowToParse, data, ',');
  double rowsAirTemp = std::stod(data);
  airTemp.insert(std::make_pair(imageNumber, rowsAirTemp));
  // std::cout << " " << rowsAirTemp;

  // Read Wa
  std::getline(rowToParse, data, ',');
  double rowsWa = std::stod(data);
  wa.insert(std::make_pair(imageNumber, rowsWa));
  // std::cout << " " << rowsWa << std::endl;
}

void ImageConverter::loadTemperatureData() {
  std::vector<std::string> list;
  DIR *rawTemperatureImagesDirectory;
  struct dirent *currentFile;

  rawTemperatureImagesDirectory = opendir(rawTemperatureDirectory.c_str());
  if (rawTemperatureImagesDirectory != NULL) {
    while ((currentFile = readdir(rawTemperatureImagesDirectory))) {
      if (currentFile->d_name[0] == '.')
        continue;
      std::string fileName = currentFile->d_name;

      std::string imageNumber = fileName.substr(fileName.find_last_of("_") + 1);
      imageNumber = imageNumber.substr(0, imageNumber.find("."));

      // std::cout << currentFile->d_name << " ";
      // std::cout << imageNumber << std::endl;

      Image temperatureImage =
          loadImageFromFile(rawTemperatureDirectory + fileName);
      // for (auto &&row : temperatureImage) {
      //   for (auto &&entry : row) {
      //     std::cout << entry << ", ";
      //   }
      //   std::cout << std::endl;
      // }

      rawTemperatureImages.insert(
          std::make_pair(std::stoi(imageNumber), temperatureImage));
    }
  }
  closedir(rawTemperatureImagesDirectory);
}

// TODO: Add error processing (invalid entry)
Image ImageConverter::loadImageFromFile(std::string fileName) {
  Image filesImage;

  std::ifstream inputFile;
  inputFile.open(fileName);
  if (!inputFile.good()) {
    std::cout << "BAD INPUT FILE: " << fileName << std::endl;
  } else {
    std::cout << "Loading file: " << fileName << std::endl;
  }

  // int i = 0;
  while (!inputFile.eof()) {
    // std::cout << "Reading row: " << ++i << std::endl;
    std::string inputLine;
    std::getline(inputFile, inputLine);
    if (!inputLine.empty()) {
      std::istringstream rowToParse(inputLine);
      std::vector<double> numbersInRow;
      for (std::string number; std::getline(rowToParse, number, ',');) {
        numbersInRow.push_back(std::stod(number));
      }
      filesImage.push_back(numbersInRow);
    }
  }

  inputFile.close();
  return filesImage;
}

////////////////// CONVERTING DATA TO CONDUCTANCE FUNCTIONS ////////////////////
void ImageConverter::calculateConductanceMaps() {
  // std::cout << "Date: " << date << std::endl;
  // std::cout << "Base working directory: " << baseWorkingDirectory <<
  // std::endl;
  // std::cout << "Base save directory: " << baseSaveDirectory << std::endl;
  // std::cout << "Raw temperature directory: " << rawTemperatureDirectory
  //           << std::endl;

  checkDataAndImageNumberCompatability();
  averageRawTemperatureImages();
  saveAveragedTemperatureImages();
  createConductanceMaps();
  saveConductanceMaps();
}

void ImageConverter::checkDataAndImageNumberCompatability() {
  for (auto &&airTempPair : airTemp) {
    if (!dataFileNumberHasMatchingTempFile(airTempPair.first)) {
      throw std::runtime_error(
          "ERROR! Data file/ temperature image mismatch. The data number " +
          std::to_string(airTempPair.first) +
          " does not have any matching temperature images. Please check "
          "these files exist, and retry running the program.");
    }
  }
}

bool ImageConverter::dataFileNumberHasMatchingTempFile(int dataFileNumber) {
  for (auto &&tempImage : rawTemperatureImages) {
    if (tempImage.first == dataFileNumber) {
      return true;
    }
  }
  return false;
}

void ImageConverter::averageRawTemperatureImages() {
  // For every unique key in the temperature images
  for (ImageMultimap::iterator it = rawTemperatureImages.begin();
       it != rawTemperatureImages.end();
       it = rawTemperatureImages.upper_bound(it->first)) {

    // Print number of images
    // std::cout << "The key: " << it->first << " has "
    //           << rawTemperatureImages.count(it->first) << " entries."
    //           << std::endl;

    // Average all images with that key.
    averageImages(it->first, rawTemperatureImages.equal_range(it->first));
  }
}

void ImageConverter::averageImages(
    int imageKey,
    std::pair<ImageMultimap::iterator, ImageMultimap::iterator> range) {

  std::cout << "Averaging temperature images labeled: " << imageKey
            << std::endl;
  // int imageHeight = kMatrix.size();
  // int imageWidth = kMatrix[0].size();
  // std::cout << "Image height : " << imageHeight << std::endl;
  // std::cout << "Image width : " << imageWidth << std::endl;

  // Create new image with blank entires
  Image averagedImage;
  // averagedImage.resize(imageHeight + 1);
  // for (auto &&row : averagedImage) {
  //   row.resize(imageWidth);
  // }

  // For every entry in the new image
  for (int row = 0; row < range.first->second.size(); ++row) {
    std::vector<double> newRow;
    for (int column = 0; column < range.first->second[row].size(); ++column) {
      // std::cout << "Image location: " << row << ", " << column << std::endl;
      // average the entries of the images with that current value

      double sumOfValues = 0.0;
      for (ImageMultimap::iterator it = range.first; it != range.second; ++it) {
        // std::cout << it->second[row][column] << ",";
        sumOfValues += it->second[row][column];
        // std::cout << "After addition" << std::endl;
      }
      double average = sumOfValues / rawTemperatureImages.count(imageKey);
      // averagedImage[row].push_back(average);
      //                       std::distance(range.first, range.second));
      newRow.push_back(average);
      // std::cout << "Average: " << average << std::endl;
    }
    averagedImage.push_back(newRow);
  }
  averagedTemperatureImages.insert(std::make_pair(imageKey, averagedImage));
}

void ImageConverter::saveAveragedTemperatureImages() {
  std::ofstream outputFile;
  std::string fileName =
      baseSaveDirectory + "AverageTempImages/" + date + "_AverageTemp_";

  for (auto &&image : averagedTemperatureImages) {
    std::string fullName = fileName + std::to_string(image.first) + ".csv";
    outputFile.open(fullName);
    std::cout << "Saving file: " << fullName << std::endl;
    if (outputFile.is_open()) {
      for (auto &&row : image.second) {
        for (auto &&entry : row) {
          outputFile << entry << ",";
        }
        outputFile << std::endl;
      }
      outputFile.close();
    } else {
      std::cout << "Error opening file" << std::endl;
    }
  }
}

void ImageConverter::saveImage(const std::string &fileName,
                               const QImage &image) {
  std::ofstream outputFile;
  outputFile.open(fileName);

  if (outputFile.is_open()) {
    for (auto &&row : image) {
      for (auto &&entry : row) {
        outputFile << entry << ",";
      }
      outputFile << std::endl;
    }
    outputFile.close();
  } else {
    std::cout << "Error opening file" << std::endl;
  }
}

// std::vector<int> ImageConverter::getImageNumbers() {
//   // std::vector<int> imageNumbers;
//   // for (auto &&image : rawTemperatureImages) {
//   //   if (std::find(imageNumbers.begin(), imageNumbers.end(), image.first)
//   ==
//   //       imageNumbers.end()) {
//   //     std::cout << image.first;
//   //     imageNumbers.push_back(image.first);
//   //   }
//   // }
//   // return imageNumbers;
//
//   std::vector<int> imageNumbers;
//   for (std::multimap<int, Image>::iterator it = rawTemperatureImages.begin();
//        it != rawTemperatureImages.end();
//        it = rawTemperatureImages.upper_bound(it->first)) {
//     std::cout << it->first;
//     imageNumbers.push_back(it->first);
//   }
//   return imageNumbers;
// }

void ImageConverter::createConductanceMaps() {}

void ImageConverter::saveConductanceMaps() {}

//////////////////////// SELECTED PIXELS CONVERSION ////////////////////////////
void ImageConverter::summarizeSelectedPixels() {
  getPixelChoicesFromUser();
  createFile();
}

void ImageConverter::getPixelChoicesFromUser() {}

void ImageConverter::createFile() {}
