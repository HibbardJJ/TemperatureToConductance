#include "ImageConverter.hpp"
#include <boost/filesystem.hpp>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <math.h>

////////////////////////////////////////////////////////////////////////////////
/* PUBLIC FUNCTIONS */

/* Constructor for the Image Converter. Takes a string that holds the base
 * working directory for all of the project. */
ImageConverter::ImageConverter(std::string baseDirectory)
    : baseWorkingDirectory(baseDirectory) {}

/* Function that starts the program's execution. Provides options to create a K
 * Matrix, create conductance maps, or input pixels to get leaflet data */
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

////////////////////////////////////////////////////////////////////////////////
/* GENERIC USER INPUT FUNCTIONS */

/* Asks the user which type of program to execute. */
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

/* Gets the date from the user. Uses this to try and guess where the rest of the
 * data is, as well as write files with date in the title */
void ImageConverter::getDate() {
  std::cout << "Please enter the date of the data used (YYYY-MM-DD)."
            << std::endl;
  std::getline(std::cin, date);
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

/* Asks for the appropriate location to save the files the program produces". */
void ImageConverter::getBaseDir() {
  std::string usrOutputMessage =
      "directory you wish to save the program's output files to";
  baseSaveDirectory = baseWorkingDirectory + "Data/" + date + "/";
  if (!checkCorrectLocation(usrOutputMessage, baseSaveDirectory)) {
    baseSaveDirectory = getCorrectLocation(usrOutputMessage);
  }
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

/* Asks for the RValue to use in the program. */
void ImageConverter::getRValue() {
  std::string rVal;
  std::cout << "Please enter the desired R Value." << std::endl;
  std::getline(std::cin, rVal);
  rValue = std::stoi(rVal);
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

/* Gets the upper left and bottom right coordinates that will be used to crop
the raw temperature data to the region of interest. */
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

/* Gets a vector of excel coordinates that the user wants to get leaflet data
 * for. */
std::vector<std::string> ImageConverter::getPixelChoicesFromUser() {
  std::cout << "Please enter a list of Excel pixel coordinates you would like "
               "to analyze. Please use a space to separate the choices."
            << std::endl;
  std::vector<std::string> coordinatesToAnalyze;
  std::string listOfCoordinates;
  std::getline(std::cin, listOfCoordinates);
  std::istringstream rowToParse(listOfCoordinates);
  for (std::string coordinate; std::getline(rowToParse, coordinate, ' ');) {
    if (!coordinate.empty()) {
      coordinatesToAnalyze.push_back(coordinate);
    }
  }
  return coordinatesToAnalyze;
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

/* Helper function to get the correct location of a file or directory if the
 * user specifies that the assumed one is incorrect. */
std::string ImageConverter::getCorrectLocation(std::string outputMessage) {
  std::string newLocation;
  std::cout << "Please enter the full path to the " << outputMessage
            << " you wish to use. " << std::endl;
  std::getline(std::cin, newLocation);
  return newLocation;
}

/* Returns true if user enters y/Y or nothing, and false if user enters n/N */
bool ImageConverter::getYesNoResponseFromUser() {
  std::string correctLocation;
  std::getline(std::cin, correctLocation);

  if (correctLocation.empty()) {
    return true;
  } else if (correctLocation[0] == 'y' || correctLocation[0] == 'Y') {
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

////////////////////////////////////////////////////////////////////////////////
/* FUNCTIONS DEALING DIRECTLY WITH CREATING A K MATRIX */

/* Runs the necessary functions to create/save a KMatrix */
void ImageConverter::runKMatrixCreationProgram() {
  std::cout << "Creating K Matrix..." << std::endl;
  getUserInputs();
  loadNecessaryKMatrixFiles();
  calculateKMatrix();
  saveImage(kMatrixLocation, kMatrix);
}

/* Loads necessary files to create a KMatrix */
void ImageConverter::loadNecessaryKMatrixFiles() {
  loadDataFile();
  loadTemperatureData();
}

/* Runs the necessary functions to calculate the KMatrix and save it to the
member variable kMatrix. */
void ImageConverter::calculateKMatrix() {
  checkDataAndImageNumberCompatability();
  averageRawTemperatureImages();
  saveAveragedTemperatureImages();
  std::map<int, Image> kMatrices;
  createMapOfKMatrices(kMatrices);
  averageKMatricesForFinal(kMatrices);
}

/* Creates a map of KMatrices given different input temperature images groups */
void ImageConverter::createMapOfKMatrices(std::map<int, Image> &kMatrices) {
  for (auto &&imagePair : averagedTemperatureImages) {
    double airTemp = getAirTemp(imagePair.first);
    Image kMatrix = getKMatrixFromTemperatureImage(airTemp, imagePair.second);
    kMatrices.insert(std::make_pair(imagePair.first, kMatrix));
  }
}

/* Averages all the different KMatrix solutions for different temperature input
 * groups */
void ImageConverter::averageKMatricesForFinal(
    const std::map<int, Image> &kMatrices) {
  for (int row = 0; row < kMatrices.begin()->second.size(); ++row) {
    std::vector<double> kMatrixRow;
    for (int column = 0; column < kMatrices.begin()->second[0].size();
         ++column) {
      double sum = 0;
      for (auto &&matrixPair : kMatrices) {
        sum += matrixPair.second[row][column];
      }
      double average = sum / kMatrices.size();
      kMatrixRow.push_back(average);
    }
    kMatrix.push_back(kMatrixRow);
  }
}

/* Calculates the value of each pixel in the KMatrix based on air temp, R. */
Image ImageConverter::getKMatrixFromTemperatureImage(double airTemp,
                                                     const Image &tempImage) {
  // K(p) = R / (T(p) - T_air)
  Image localKMatrix;
  for (auto &&row : tempImage) {
    std::vector<double> rowInKMatrix;
    for (auto &&pixelTemp : row) {
      double pixelKValue = rValue / (pixelTemp - airTemp);
      rowInKMatrix.push_back(pixelKValue);
    }
    localKMatrix.push_back(rowInKMatrix);
  }
  return localKMatrix;
}

////////////////////////////////////////////////////////////////////////////////
/* FUNCTIONS DEALING DIRECTLY WITH CREATING CONDUCTANCE MAPS */

// Runs the necessary functions to create and save conductance maps / pixel
// summaries.
void ImageConverter::runConductanceMapCreationProgram() {
  getUserInputs();
  loadNecessaryConductanceProgramFiles();
  calculateConductanceMaps();
  summarizeSelectedPixels();
}

// Loads files necessary to create conductance maps
void ImageConverter::loadNecessaryConductanceProgramFiles() {
  std::cout << "Loading necessary files. This could take a while..."
            << std::endl;
  loadKMatrix();
  loadDataFile();
  loadTemperatureData();
}

// Runs functions necessary to confirm data input is compatible, average temp
// images, and create/save conductance maps.
void ImageConverter::calculateConductanceMaps() {
  checkDataAndImageNumberCompatability();
  averageRawTemperatureImages();
  saveAveragedTemperatureImages();
  createConductanceMaps();
}

// Creates and saves the conductance maps.
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
    conductanceImages.insert(
        std::make_pair(tempImagePair.first, conductanceImage));
    saveImage(fullFileName, conductanceImage);
  }
}

// Creates a particular conductance map.
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

// Calculates the conductance of a single pixel.
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

// Gets pixels user would like to save data for, gathers and saves that data.
void ImageConverter::summarizeSelectedPixels() {
  std::cout << "Would you like to pull data about particular leaflets? [y/n]"
            << std::endl;
  bool answer = getYesNoResponseFromUser();
  if (answer) {
    std::vector<std::string> coordinatesToAnalyze = getPixelChoicesFromUser();
    createSelectedPixelsFile(coordinatesToAnalyze);
  }
}

////////////////////////////////////////////////////////////////////////////////
/* FUNCTIONS DEALING WITH PATCH DATA CALCULATION PROGRAM EXECUTION */

// Runs the necessary functions to load necessary files to gather a
// pixel/leaflet summary.
void ImageConverter::runPixelSummaryProgram() {
  topLeftWindowCoordinate = Coordinate(0, 0);
  bottomRightWindowCoordinate = Coordinate(100000, 100000);
  getDate();
  getBaseDir();
  getProgramInputFile();
  loadDataFile();
  readAverageTemperatureImages();
  readConductanceImages();
  std::vector<std::string> coordinatesToAnalyze = getPixelChoicesFromUser();
  createSelectedPixelsFile(coordinatesToAnalyze);
}

// Reads in average temperature images created by a previous program execution.
void ImageConverter::readAverageTemperatureImages() {
  DIR *averageTemperatureImagesDirectory;
  struct dirent *currentFile;

  std::string averageTempDirectoryName =
      baseSaveDirectory + "AverageTempImages/";
  averageTemperatureImagesDirectory = opendir(averageTempDirectoryName.c_str());
  if (averageTemperatureImagesDirectory != NULL) {
    while ((currentFile = readdir(averageTemperatureImagesDirectory))) {
      if (currentFile->d_name[0] == '.')
        continue;
      else {
        std::string fileName = averageTempDirectoryName + currentFile->d_name;
        std::string imageNumber =
            fileName.substr(fileName.find_last_of("_") + 1);
        imageNumber = imageNumber.substr(0, imageNumber.find("."));
        Image tempImage = loadImageFromFile(fileName);
        averagedTemperatureImages.insert(
            std::make_pair(std::stoi(imageNumber), tempImage));
      }
    }
  }
  closedir(averageTemperatureImagesDirectory);
}

// Reads in conductance images created by a previous program execution.
void ImageConverter::readConductanceImages() {
  DIR *conductanceImageDirectory;
  struct dirent *currentFile;

  std::string conductanceDirectoryName =
      baseSaveDirectory + "ConductanceImages/";
  conductanceImageDirectory = opendir(conductanceDirectoryName.c_str());
  if (conductanceImageDirectory != NULL) {
    while ((currentFile = readdir(conductanceImageDirectory))) {
      if (currentFile->d_name[0] == '.')
        continue;
      else {
        std::string fileName = conductanceDirectoryName + currentFile->d_name;
        std::string imageNumber =
            fileName.substr(fileName.find_last_of("_") + 1);
        imageNumber = imageNumber.substr(0, imageNumber.find("."));
        Image conductanceImage = loadImageFromFile(fileName);
        conductanceImages.insert(
            std::make_pair(std::stoi(imageNumber), conductanceImage));
      }
    }
  }
  closedir(conductanceImageDirectory);
}

////////////////////////////////////////////////////////////////////////////////
/* FUNCTIONS DEALING WITH LOADING DATA FROM FILES */

// Loads KMatrix to local variable kMatrix.
void ImageConverter::loadKMatrix() {
  kMatrix = loadImageFromFile(kMatrixLocation);
}

// Loads Data File to appropriate local variables wa and airTemp.
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

// Parses a single line on the Data File.
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

// Loads the raw temperature data from the filesystem.
void ImageConverter::loadTemperatureData() {
  DIR *rawTemperatureImagesDirectory;
  struct dirent *currentFile;

  rawTemperatureImagesDirectory = opendir(rawTemperatureDirectory.c_str());
  if (rawTemperatureImagesDirectory != NULL) {
    while ((currentFile = readdir(rawTemperatureImagesDirectory))) {
      if (currentFile->d_name[0] == '.')
        continue;
      else {
        std::string fileName = rawTemperatureDirectory + currentFile->d_name;
        std::string imageNumber =
            fileName.substr(fileName.find_last_of("_") + 1);
        imageNumber = imageNumber.substr(0, imageNumber.find("."));
        if (confirmShouldLoadImageBasedOnDataFile(imageNumber)) {
          Image tempImage = loadImageFromFile(fileName);
          rawTemperatureImages.insert(
              std::make_pair(std::stoi(imageNumber), tempImage));
        }
      }
    }
    closedir(rawTemperatureImagesDirectory);
  }
}

// Double checks that the file should be loaded (its number is specified in data
// file).
bool ImageConverter::confirmShouldLoadImageBasedOnDataFile(
    std::string imageNumber) {
  int number = std::stoi(imageNumber);
  if (airTemp.count(number)) {
    return true;
  } else {
    return false;
  }
}

// Returns the image that is stored at the file location fileName.
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

// Parses a single csv row of numbers.
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

////////////////////////////////////////////////////////////////////////////////
/* FUNCTIONS DEALING WITH SAVING IMAGES TO FILES */

// Saves an Image to the specified file.
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

// Saves the whole group of average temperature images.
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

////////////////////////////////////////////////////////////////////////////////
/* FUNCTIONS DEALING WITH SAVING PIXEL DATA TO FILES */

// Creates the file that holds leaflet data, based on users preferences.
void ImageConverter::createSelectedPixelsFile(
    const std::vector<std::string> &coordinates) {
  std::ofstream outputFile;
  outputFile.open(baseSaveDirectory + "/PixelAnalysis.csv");
  if (outputFile.is_open()) {
    for (auto &&excelCoordinate : coordinates) {
      outputFile << "Excel Coordinate:," << excelCoordinate << std::endl;
      Coordinate coordinate = convertExcelNumberToStandard(excelCoordinate);
      writeCoordinateHeader(outputFile, coordinate);
      printParticularPixelData(outputFile, coordinate);
      outputFile << std::endl;
    }
  }
}

// Writes the header for each particular selected pixel.
void ImageConverter::writeCoordinateHeader(std::ofstream &outputFile,
                                           const Coordinate &coordinate) {
  outputFile << "Standard X Coordinate:," << coordinate.first << std::endl;
  outputFile << "Standard Y Coordiante:," << coordinate.second << std::endl;
  outputFile << "Image Number,, Pixel Temp , Pixel DeltaW , Pixel "
                "Conductance ,, Leaflet Temp, Leaflet DeltaW, Leaflet "
                "Conductance"
             << std::endl;
}

// Prints the desired data (temp, conductance, delta w) for each pixel/leaflet.
void ImageConverter::printParticularPixelData(std::ofstream &outputFile,
                                              const Coordinate &coordinate) {
  std::cout << "Printing coordinate: (" << coordinate.first << ","
            << coordinate.second << ")" << std::endl;
  for (auto &&conductanceImage : conductanceImages) {
    int imageNumber = conductanceImage.first;

    // Print image number
    outputFile << imageNumber << ",,";

    // Print pixel temp
    double pixelTemp = getPixelTemp(imageNumber, coordinate);
    outputFile << pixelTemp << ",";

    // Print pixel delta w
    double waValue = getWaValue(imageNumber);
    outputFile << getWpValue(pixelTemp) - waValue << ",";

    // Print pixel conductance
    outputFile
        << (conductanceImage.second).at(coordinate.second).at(coordinate.first)
        << ",,";

    // Print leaflet temp
    double leafletTemp = getLeafletTemp(imageNumber, coordinate);
    outputFile << leafletTemp << ",";

    // TODO: Print leaflet delta w
    outputFile << getWpValue(leafletTemp) - waValue << ",";

    // Print leaflet conductance
    outputFile << getLeafletConductance(conductanceImage.second, coordinate)
               << std::endl;
  }
}

////////////////////////////////////////////////////////////////////////////////
/* FUNCTIONS DEALING WITH DATA INPUT COMPATIBILITY */

// Checks that every entry in the data file has a temperature image to do the
// calculation with.
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

// Helper file that confirms there is a temperature file with a given key.
bool ImageConverter::dataFileNumberHasMatchingTempFile(int dataFileNumber) {
  for (auto &&tempImage : rawTemperatureImages) {
    if (tempImage.first == dataFileNumber) {
      return true;
    }
  }
  return false;
}

////////////////////////////////////////////////////////////////////////////////
/* FUNCTIONS DEALING WITH AVERING A SERIES OF IMAGES */

// Averages all the raw temperature images groups.
void ImageConverter::averageRawTemperatureImages() {
  // For every unique key in the temperature images
  for (ImageMultimap::iterator it = rawTemperatureImages.begin();
       it != rawTemperatureImages.end();
       it = rawTemperatureImages.upper_bound(it->first)) {
    // Average all images with that key.
    averageImages(it->first, rawTemperatureImages.equal_range(it->first));
  }
}

// Averages a single group of temperature images and saves it to member variable
// averagedImage.
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

////////////////////////////////////////////////////////////////////////////////
/* FUNCTIONS DEALING WITH CONVERTING FROM EXCEL COORDINATES TO STANDARD */

// Returns the standard coordinate (starting at 0,0) of an excel coordinate.
Coordinate ImageConverter::convertExcelNumberToStandard(std::string number) {
  // parse into ABC section (x direction) and 123 section (y direction).
  auto locationOfFirstNumber = number.find_first_of("1234567890");
  auto excelXCoordinate = number.substr(0, locationOfFirstNumber);
  auto excelYCoordinate = number.substr(locationOfFirstNumber);
  int rawXCoordinate =
      convertExcelXCoordinate(number.substr(0, locationOfFirstNumber));
  int rawYCoordinate = std::stoi(excelYCoordinate) - 1;

  return Coordinate(rawXCoordinate, rawYCoordinate);
}

// Converts just the x coordinate (ABC..) of an excel coordinate to standard.
int ImageConverter::convertExcelXCoordinate(std::string excelXCoordinate) {
  int stringLength = excelXCoordinate.size();
  int sum = 0;
  for (int i = 0; i < stringLength; ++i) {
    auto charValue = getCharValue(excelXCoordinate[i]);
    sum += charValue * pow(26, stringLength - 1 - i);
  }
  return sum - 1;
}

// Gets the value of a specific in base 26 letters.
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

////////////////////////////////////////////////////////////////////////////////
/* FUNCTIONS DEALING WITH GETTING NECESSARY PARAMETERS FOR EQUATIONS */

// Gets the air temperature associated with a specific image number (from data
// file).
double ImageConverter::getAirTemp(int imageNumber) {
  auto it = airTemp.find(imageNumber);
  if (it != airTemp.end()) {
    return it->second;
  } else {
    throw std::runtime_error(
        "Temperature image does not have corresponding air temp value.");
  }
}

// Get the wa value associated with a specific image number (from data file).
double ImageConverter::getWaValue(int imageNumber) {
  auto it = wa.find(imageNumber);
  if (it != wa.end()) {
    return it->second;
  } else {
    throw std::runtime_error(
        "Temperature image does not have corresponding wa value.");
  }
}

// Gets the wp value of a pixel given its temperature.
double ImageConverter::getWpValue(double pixelTemp) {
  // w(p) = w0 * exp( -Tw / T(p))

  const double w0 = 6.57959 * pow(10, 8);
  const double Tw = 4982.85;

  return w0 * exp(-Tw / (pixelTemp + 273.15));
}

// Gets the temperature of a pixel given its image number and coordinate.
double ImageConverter::getPixelTemp(int imageNumber,
                                    const Coordinate &coordinate) {

  auto location = averagedTemperatureImages.find(imageNumber);
  if (location != averagedTemperatureImages.end()) {
    return (location->second).at(coordinate.second).at(coordinate.first);
  } else {
    throw std::runtime_error("Unexpected error saving leaflet data. "
                             "Conducatance number and temperature number do "
                             "not match.");
  }
}

// Gets the average temperature of a 9 pixel leaflet centered at the coordinate,
// and with image number specified.
double ImageConverter::getLeafletTemp(int imageNumber,
                                      const Coordinate &coordinate) {
  auto location = averagedTemperatureImages.find(imageNumber);
  if (location != averagedTemperatureImages.end()) {
    Image tempImage = location->second;
    double sum = 0.0;
    sum += tempImage.at(coordinate.second - 1).at(coordinate.first - 1);
    sum += tempImage.at(coordinate.second - 1).at(coordinate.first);
    sum += tempImage.at(coordinate.second - 1).at(coordinate.first + 1);
    sum += tempImage.at(coordinate.second).at(coordinate.first - 1);
    sum += tempImage.at(coordinate.second).at(coordinate.first);
    sum += tempImage.at(coordinate.second).at(coordinate.first + 1);
    sum += tempImage.at(coordinate.second + 1).at(coordinate.first - 1);
    sum += tempImage.at(coordinate.second + 1).at(coordinate.first);
    sum += tempImage.at(coordinate.second + 1).at(coordinate.first + 1);
    return sum / 9.0;
  } else {
    throw std::runtime_error("Unexpected error saving leaflet data. "
                             "Conducatance number and temperature number do "
                             "not match.");
  }
}

// Gets the average conductance of a 9 pixel leaflet centered at the coordinate,
// and with image number specified.
double ImageConverter::getLeafletConductance(const Image &conductanceImage,
                                             const Coordinate &coordinate) {
  double sum = 0.0;
  sum += conductanceImage.at(coordinate.second - 1).at(coordinate.first - 1);
  sum += conductanceImage.at(coordinate.second - 1).at(coordinate.first);
  sum += conductanceImage.at(coordinate.second - 1).at(coordinate.first + 1);
  sum += conductanceImage.at(coordinate.second).at(coordinate.first - 1);
  sum += conductanceImage.at(coordinate.second).at(coordinate.first);
  sum += conductanceImage.at(coordinate.second).at(coordinate.first + 1);
  sum += conductanceImage.at(coordinate.second + 1).at(coordinate.first - 1);
  sum += conductanceImage.at(coordinate.second + 1).at(coordinate.first);
  sum += conductanceImage.at(coordinate.second + 1).at(coordinate.first + 1);
  return sum / 9.0;
}
