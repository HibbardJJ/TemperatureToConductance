#include "ImageConverter.hpp"
#include <boost/filesystem.hpp>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <math.h>

/* Constructor for the Image Converter. Takes a string that holds the base
 * working directory for all of the project. */
ImageConverter::ImageConverter(std::string baseDirectory)
    : baseWorkingDirectory(baseDirectory) {}

///////////////////////// INPUT FUNCTIONS //////////////////////////////////////

int ImageConverter::getProgramExecutionType() {
  std::cout << "What type of program would you like to run?" << std::endl;
  std::cout << "\tEnter '1' to create a K Matrix." << std::endl;
  std::cout << "\tEnter '2' to create Conductance Maps." << std::endl;
  std::cout << "\tEnter '3' to analyze patches of previously created "
               "conductance maps."
            << std::endl;
  std::string choice;
  std::getline(std::cin, choice);
  return std::stoi(choice);
}

void ImageConverter::chooseProgramTypeAndExecute() {
  int choice = getProgramExecutionType();
  switch (choice) {
  case 1:
    runKMatrixCreationProgram();
    break;
  case 2:
    runConductanceMapCreationProgram();
    break;
  case 3:
    runPixelSummaryProgram();
    break;
  }
}

void ImageConverter::runConductanceMapCreationProgram() {
  getUserInputs();
  loadNecessaryFiles();
  calculateConductanceMaps();
  summarizeSelectedPixels();
}

void ImageConverter::runKMatrixCreationProgram() {
  std::cout << "Creating K Matrix..." << std::endl;
}

void ImageConverter::runPixelSummaryProgram() {
  std::cout << "Calculating Pixel Information..." << std::endl;
}

/* Function that gets all the necessary inputs from the user. */
void ImageConverter::getUserInputs() {
  getDate();
  getRValue();
  getWindowCoordinates();
  getKMatrix();
  getRawTemperatureDir();
  getProgramInputFile();
  getBaseDir();
}

/* Gets the date from the user. Uses this to try and guess where the rest of the
 * data is, as well as write files with date in the title */
void ImageConverter::getDate() {
  std::cout << "Please enter the date of the data used (YYYY-MM-DD)."
            << std::endl;
  std::getline(std::cin, date);
}

void ImageConverter::getWindowCoordinates() {
  std::string topLeftCoordinate = "EX72";
  std::string bottomRightCoordinate = "VN434";

  std::cout << "Are the window coordinates you'd like to crop the images to : "
               "( " +
                   topLeftCoordinate + ", " + bottomRightCoordinate +
                   " )? [Enter y/n]"
            << std::endl;
  bool answeredYes = getYesNoResponseFromUser();
  if (!answeredYes) {
    std::cout << "Please enter the Excel coordinate of the top left pixel of "
                 "the window."
              << std::endl;
    std::getline(std::cin, topLeftCoordinate);
    std::cout << "Please enter the Excel coordinate of the bottom right pixel "
                 "of the window."
              << std::endl;
    std::getline(std::cin, bottomRightCoordinate);
  }

  topLeftWindowCoordinate = convertExcelNumberToStandard(topLeftCoordinate);
  bottomRightWindowCoordinate =
      convertExcelNumberToStandard(bottomRightCoordinate);
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
  std::cout << "Is the location of the " << outputMessage << " you wish to use \
  \'" << assumedLocation
            << "\'? [Enter y/n]" << std::endl;
  return getYesNoResponseFromUser();
}

bool ImageConverter::getYesNoResponseFromUser() {
  std::string correctLocation;
  std::getline(std::cin, correctLocation);

  if (correctLocation[0] == 'y' || correctLocation[0] == 'Y') {
    return true;
  } else if (correctLocation[0] == 'n' || correctLocation[0] == 'N') {
    return false;
  } else {
    std::cout
        << "INVALID RESPONSE. Please enter either 'y' or 'n'. Please try again."
        << std::endl;
    return getYesNoResponseFromUser();
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
}

void ImageConverter::parseInputFileLine(std::istringstream &rowToParse) {
  std::string data;

  // Read image number
  std::getline(rowToParse, data, ',');
  int imageNumber = std::stoi(data);

  // Read air temperature;
  std::getline(rowToParse, data, ',');
  double rowsAirTemp = std::stod(data);
  airTemp.insert(std::make_pair(imageNumber, rowsAirTemp));

  // Read Wa
  std::getline(rowToParse, data, ',');
  double rowsWa = std::stod(data);
  wa.insert(std::make_pair(imageNumber, rowsWa));
}

void ImageConverter::loadTemperatureData() {
  DIR *rawTemperatureImagesDirectory;
  struct dirent *currentFile;

  rawTemperatureImagesDirectory = opendir(rawTemperatureDirectory.c_str());
  if (rawTemperatureImagesDirectory != NULL) {
    while ((currentFile = readdir(rawTemperatureImagesDirectory))) {
      if (currentFile->d_name[0] == '.')
        continue;
      else {
        loadTempFile(currentFile->d_name);
      }
    }
  }
  closedir(rawTemperatureImagesDirectory);
}

void ImageConverter::loadTempFile(std::string fileName) {
  std::string imageNumber = fileName.substr(fileName.find_last_of("_") + 1);
  imageNumber = imageNumber.substr(0, imageNumber.find("."));

  if (confirmShouldLoadImageBasedOnDataFile(imageNumber)) {
    Image temperatureImage =
        loadImageFromFile(rawTemperatureDirectory + fileName);

    rawTemperatureImages.insert(
        std::make_pair(std::stoi(imageNumber), temperatureImage));
  }
}

bool ImageConverter::confirmShouldLoadImageBasedOnDataFile(
    std::string imageNumber) {
  int number = std::stoi(imageNumber);
  if (airTemp.count(number)) {
    return true;
  } else {
    return false;
  }
}

Image ImageConverter::loadImageFromFile(std::string fileName) {
  Image filesImage;

  std::ifstream inputFile;
  inputFile.open(fileName);
  if (!inputFile.good()) {
    std::cout << "BAD INPUT FILE: " << fileName << std::endl;
  } else {
    std::cout << "Loading file: " << fileName << std::endl;
  }

  int rowNumber = 0;
  while (!inputFile.eof()) {
    std::string inputLine;
    std::getline(inputFile, inputLine);
    ++rowNumber;
    if (rowNumber < topLeftWindowCoordinate.second) {
      continue;
    } else if (rowNumber > bottomRightWindowCoordinate.second) {
      break;
    }
    auto numbersInRow = parseRow(inputLine);
    if (!numbersInRow.empty()) {
      filesImage.push_back(numbersInRow);
    }
  }
  inputFile.close();
  return filesImage;
}

std::vector<double> ImageConverter::parseRow(std::string &inputLine) {
  std::vector<double> numbersInRow;

  if (!inputLine.empty()) {
    std::istringstream rowToParse(inputLine);

    int columnNumber = 0;
    for (std::string number; std::getline(rowToParse, number, ',');) {
      ++columnNumber;
      if (columnNumber < topLeftWindowCoordinate.first) {
        continue;
      } else if (columnNumber > bottomRightWindowCoordinate.first) {
        break;
      }
      numbersInRow.push_back(std::stod(number));
    }
  }
  return numbersInRow;
}

////////////////// CONVERTING DATA TO CONDUCTANCE FUNCTIONS ////////////////////
void ImageConverter::calculateConductanceMaps() {
  checkDataAndImageNumberCompatability();
  averageRawTemperatureImages();
  saveAveragedTemperatureImages();
  createConductanceMaps();
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
    // Average all images with that key.
    averageImages(it->first, rawTemperatureImages.equal_range(it->first));
  }
}

void ImageConverter::averageImages(
    int imageKey,
    std::pair<ImageMultimap::iterator, ImageMultimap::iterator> range) {

  std::cout << "Averaging temperature images labeled: " << imageKey
            << std::endl;

  // Create new image that will hold the averaged temperature images
  Image averagedImage;

  // For every entry in the new image
  for (int row = 0; row < range.first->second.size(); ++row) {
    std::vector<double> newRow;
    for (int column = 0; column < range.first->second[row].size(); ++column) {
      // Sum the values
      double sumOfValues = 0.0;
      for (ImageMultimap::iterator it = range.first; it != range.second; ++it) {
        sumOfValues += it->second[row][column];
      }
      // Then average them
      double average = sumOfValues / rawTemperatureImages.count(imageKey);
      newRow.push_back(average);
    }
    averagedImage.push_back(newRow);
  }

  // Add new image to list to use
  averagedTemperatureImages.insert(std::make_pair(imageKey, averagedImage));
}

void ImageConverter::saveAveragedTemperatureImages() {
  std::ofstream outputFile;
  boost::filesystem::path dir(baseSaveDirectory + "AverageTempImages/");
  boost::filesystem::create_directory(dir);

  std::string fileName =
      baseSaveDirectory + "AverageTempImages/" + date + "_AverageTemp_";

  for (auto &&image : averagedTemperatureImages) {
    std::string fullName = fileName + std::to_string(image.first) + ".csv";
    saveImage(fullName, image.second);
  }
}

void ImageConverter::saveImage(const std::string &fileName,
                               const Image &image) {
  std::ofstream outputFile;
  outputFile.open(fileName);

  if (outputFile.is_open()) {
    std::cout << "Saving file: " << fileName << std::endl;
    for (auto &&row : image) {
      for (auto &&entry : row) {
        outputFile << entry << ",";
      }
      outputFile << std::endl;
    }
    outputFile.close();
  } else {
    throw std::runtime_error("ERROR OPENING FILE: " + fileName);
    // std::cout << "ERROR OPENING FILE: " << fileName << std::endl;
  }
}

void ImageConverter::createConductanceMaps() {
  std::string fileName =
      baseSaveDirectory + "ConductanceImages/" + date + "_Conductance_";
  boost::filesystem::path dir(baseSaveDirectory + "ConductanceImages/");
  boost::filesystem::create_directory(dir);
  for (auto &&tempImagePair : averagedTemperatureImages) {
    Image conductanceImage =
        createConductanceImage(tempImagePair.first, tempImagePair.second);
    std::string fullFileName =
        fileName + std::to_string(tempImagePair.first) + ".csv";
    saveImage(fullFileName, conductanceImage);
  }
}

Image ImageConverter::createConductanceImage(int imageNumber,
                                             const Image &tempImage) {
  Image conductanceImage;
  for (int row = 0; row < tempImage.size(); ++row) {
    std::vector<double> newRow;
    for (int column = 0; column < tempImage.at(row).size(); ++column) {
      newRow.push_back(calculateConductance(imageNumber, row, column,
                                            tempImage.at(row).at(column)));
    }
    conductanceImage.push_back(newRow);
  }
  return conductanceImage;
}

double ImageConverter::calculateConductance(int imageNumber, int row,
                                            int column, double pixelTemp) {

  // g = ( R + K(Ta - Tp) ) / ( Lw * (wp - wa) )
  const double Lw = 40.68;
  double K = kMatrix.at(row).at(column);
  double Ta = getAirTemp(imageNumber);
  double Wa = getWaValue(imageNumber);
  double Wp = getWpValue(pixelTemp);

  double numerator = rValue + K * (Ta - pixelTemp);
  double denominator = Lw * (Wp - Wa);
  return numerator / denominator;
}

double ImageConverter::getAirTemp(int imageNumber) {
  auto it = airTemp.find(imageNumber);
  if (it != airTemp.end()) {
    return it->second;
  } else {
    throw std::runtime_error(
        "Temperature image does not have corresponding air temp value.");
  }
}

double ImageConverter::getWaValue(int imageNumber) {
  auto it = wa.find(imageNumber);
  if (it != wa.end()) {
    return it->second;
  } else {
    throw std::runtime_error(
        "Temperature image does not have corresponding wa value.");
  }
}

double ImageConverter::getWpValue(double pixelTemp) {
  // w(p) = w0 * exp( -Tw / T(p))

  const double w0 = 6.57959 * pow(10, 8);
  const double Tw = 4982.85;

  return w0 * exp(-Tw / (pixelTemp + 273.15));
}

//////////////////////// SELECTED PIXELS CONVERSION
/////////////////////////////
std::pair<int, int>
ImageConverter::convertExcelNumberToStandard(std::string number) {
  // parse into ABC section (x direction) and 123 section (y direction).
  auto locationOfFirstNumber = number.find_first_of("1234567890");
  auto excelXCoordinate = number.substr(0, locationOfFirstNumber);
  auto excelYCoordinate = number.substr(locationOfFirstNumber);
  int rawXCoordinate =
      convertExcelXCoordinate(number.substr(0, locationOfFirstNumber));
  int rawYCoordinate = std::stoi(excelYCoordinate) - 1;

  std::cout << "Excel X Coordinate: " << excelXCoordinate << std::endl;
  std::cout << "Excel Y Coordinate: " << excelYCoordinate << std::endl;
  std::cout << "Raw X Coordinate: " << rawXCoordinate << std::endl;
  std::cout << "Raw Y Coordinate: " << rawYCoordinate << std::endl;

  return std::pair<int, int>(rawXCoordinate, rawYCoordinate);
}

int ImageConverter::convertExcelXCoordinate(std::string excelXCoordinate) {
  int stringLength = excelXCoordinate.size();
  int sum = 0;
  for (int i = 0; i < stringLength; ++i) {
    auto charValue = getCharValue(excelXCoordinate[i]);
    sum += charValue * pow(26, stringLength - 1 - i);
  }
  return sum - 1;
}

int ImageConverter::getCharValue(char character) {
  if (character >= 'A' && character <= 'Z') {
    return character - 'A' + 1;
  } else if (character >= 'a' && character <= 'z') {
    return character - 'a' + 1;
  } else {
    std::string message = "Do not recognize excel x coordinate: '";
    message.push_back(character);
    message.append("'");
    throw std::runtime_error(message);
  }
}

void ImageConverter::summarizeSelectedPixels() {
  getPixelChoicesFromUser();
  createFile();
}

void ImageConverter::getPixelChoicesFromUser() {}

void ImageConverter::createFile() {}
