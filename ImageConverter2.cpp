#include "ImageConverter2.hpp"
#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>

////////////////////////////////////////////////////////////////////////////////
/* CONSTRUCTOR */

ImageConverter::ImageConverter(const Path &pathToBaseDirectory) {
  int choice = getProgramExecutionType();
  switch (choice) {
  case 1:
    runKMatrixCreationProgram(pathToBaseDirectory);
    break;
  case 2:
    runConductanceMapCreationProgram(pathToBaseDirectory);
    break;
  }
}

////////////////////////////////////////////////////////////////////////////////
/* MAIN PROGRAM EXECUTION */

void ImageConverter::runKMatrixCreationProgram(
    const Path &pathToBaseDirectory) {
  std::cout << "Starting KMatrix Creation Program" << std::endl;
  initializeVariablesForKMatrixProgram(pathToBaseDirectory);
  confirmKMatrixCreationVariableInitializationIsCorrect();
  iterateThroughKMatrixDirectoriesAndCreate();
}

void ImageConverter::runConductanceMapCreationProgram(
    const Path &pathToBaseDirectory) {
  std::cout << "Starting Conductance Map Creation Program" << std::endl;
  initializeVariablesForConductanceMapProgram(pathToBaseDirectory);
  confirmConductanceMapVariableInitializationIsCorrect();
  loadAllConductanceProgramData();
  saveAverageTemperatureImages();
  createConductanceMaps();
  summarizeSelectedPixels();
}

////////////////////////////////////////////////////////////////////////////////
/* PROGRAM VARIABLE INITIALIZATION */

/* KMatrix Program */
void ImageConverter::initializeVariablesForKMatrixProgram(
    const Path &pathToBaseDirectory) {
  date = "";
  std::string basePath = pathToBaseDirectory.generic_string();
  baseSaveDirectory = Path(basePath + "KMatrix/");
  programDataInputFile = Path(basePath + "KMatrix/DataExtraction.csv");
  temperatureImagesDirectory = Path(basePath + "KMatrix/"); // Shouldn't be used
  kMatrixDirectory = Path(basePath + "KMatrix/");
  topLeftWindowCoordinate = convertExcelNumberToStandard("EX72");
  bottomRightWindowCoordinate = convertExcelNumberToStandard("VN434");
}

/* Conductance Map Program */
void ImageConverter::initializeVariablesForConductanceMapProgram(
    const Path &pathToBaseDirectory) {
  std::string basePath = pathToBaseDirectory.generic_string();
  getDateFromUser();
  baseSaveDirectory = Path(basePath + "Data/" + date + "/");
  programDataInputFile =
      Path(basePath + "Data/" + date + "/DataExtraction.csv");
  temperatureImagesDirectory = Path(basePath + "Data/" + date + "/TempImages/");
  kMatrixDirectory = Path(basePath + "KMatrix/");
  topLeftWindowCoordinate = convertExcelNumberToStandard("EX72");
  bottomRightWindowCoordinate = convertExcelNumberToStandard("VN434");
}

////////////////////////////////////////////////////////////////////////////////
/* CONFIRM INITIALIZED VARIABLES ARE CORRECT */

void ImageConverter::confirmConductanceMapVariableInitializationIsCorrect() {
  getRValueFromUser();
  confirmBaseSaveDirectoryPathIsCorrect();
  confirmKMatrixDirectoryPathIsCorrect();
  confirmProgramDataInputFilePathIsCorrect();
  confirmTemperatureFilesPathIsCorrect();
  confirmCropImageCoordinatesAreCorrect();
}

void ImageConverter::confirmKMatrixCreationVariableInitializationIsCorrect() {
  confirmKMatrixDirectoryPathIsCorrect();
  confirmCropImageCoordinatesAreCorrect();
}

void ImageConverter::confirmBaseSaveDirectoryPathIsCorrect() {
  if (!askIfPathIsCorrectForFile("base data directory", baseSaveDirectory)) {
    baseSaveDirectory = getCorrectPathFromUser();
  }
}

void ImageConverter::confirmKMatrixDirectoryPathIsCorrect() {
  if (!askIfPathIsCorrectForFile("K Matrix directory", kMatrixDirectory)) {
    kMatrixDirectory = getCorrectPathFromUser();
  }
}

void ImageConverter::confirmProgramDataInputFilePathIsCorrect() {
  if (!askIfPathIsCorrectForFile("data input file", programDataInputFile)) {
    programDataInputFile = getCorrectPathFromUser();
  }
}

void ImageConverter::confirmTemperatureFilesPathIsCorrect() {
  if (!askIfPathIsCorrectForFile("temperature images directory",
                                 temperatureImagesDirectory)) {
    temperatureImagesDirectory = getCorrectPathFromUser();
  }
}

bool ImageConverter::askIfPathIsCorrectForFile(const std::string &message,
                                               const Path &path) {
  std::cout << "Is the correct path for the " << message << " " << path << "?"
            << std::endl;
  return getYesNoResponseFromUser();
}

Path ImageConverter::getCorrectPathFromUser() {
  std::string newLocation;
  std::cout << "Please enter the full path you wish to use instead."
            << std::endl;
  std::getline(std::cin, newLocation);
  return Path(newLocation);
}

/* Gets the upper left and bottom right coordinates that will be used to crop
the raw temperature data to the region of interest. */
void ImageConverter::confirmCropImageCoordinatesAreCorrect() {
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

////////////////////////////////////////////////////////////////////////////////
/* BASIC USER INPUT COMMUNICATION */

/* Asks the user which type of program to execute. */
int ImageConverter::getProgramExecutionType() {
  std::cout << "What type of program would you like to run?" << std::endl;
  std::cout << "\tEnter '1' to create a K Matrix." << std::endl;
  std::cout << "\tEnter '2' to create Conductance Maps." << std::endl;
  std::string choice;
  std::getline(std::cin, choice);
  return std::stoi(choice);
}

void ImageConverter::getDateFromUser() {
  std::cout << "Please enter the date of the data used (YYYY-MM-DD)."
            << std::endl;
  std::getline(std::cin, date);
}

void ImageConverter::getRValueFromUser() {
  std::string rVal;
  std::cout << "Please enter the desired R Value." << std::endl;
  std::getline(std::cin, rVal);
  rValue = std::stoi(rVal);
}

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

////////////////////////////////////////////////////////////////////////////////
/* LOAD NECESSARY DATA */

void ImageConverter::loadAllConductanceProgramData() {
  std::ifstream inputFile;
  std::cout << "Loading file: " << programDataInputFile << std::endl;
  inputFile.open(programDataInputFile.string());

  std::string inputLine;
  while (!inputFile.eof()) {
    std::getline(inputFile, inputLine);
    // std::cout << inputLine << std::endl;
    if (!inputLine.empty()) {
      std::istringstream rowToParse(inputLine);
      parseInputFileLine(rowToParse);
    }
  }
}

void ImageConverter::parseInputFileLine(std::istringstream &rowToParse) {
  std::string data;

  // Get temperature image identifier
  std::getline(rowToParse, data, ',');
  std::string imageIdentifier = data;
  // std::cout << "Identifier: " << imageIdentifier << std::endl;
  loadTemperatureImagesWithIdentifier(imageIdentifier);

  // Get KMatrix image identifier
  std::getline(rowToParse, data, ',');
  loadKMatrixWithIdentifier(imageIdentifier, data);

  // Read four thermocouple temperatures
  std::getline(rowToParse, data, ',');
  double upperBeforeThermocouple = std::stod(data);

  std::getline(rowToParse, data, ',');
  double upperAfterThermocouple = std::stod(data);

  std::getline(rowToParse, data, ',');
  double lowerBeforeThermocouple = std::stod(data);

  std::getline(rowToParse, data, ',');
  double lowerAfterThermocouple = std::stod(data);

  auto airTempPair =
      loadAirTemperatures(upperBeforeThermocouple, upperAfterThermocouple,
                          lowerBeforeThermocouple, lowerAfterThermocouple);
  airTemps.insert(std::make_pair(imageIdentifier, airTempPair));

  // Read Wa
  std::getline(rowToParse, data, ',');
  wa.insert(std::make_pair(imageIdentifier, std::stod(data)));
}

Image ImageConverter::loadImageFromFile(const Path &path) {
  Image filesImage;

  std::ifstream inputFile;
  inputFile.open(path.string());
  if (!inputFile.good()) {
    std::cout << "BAD INPUT FILE: " << path << std::endl;
  } else {
    std::cout << "Loading file: " << path << std::endl;
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
    auto numbersInRow = parseImageFileRow(inputLine);
    if (!numbersInRow.empty()) {
      filesImage.push_back(numbersInRow);
    }
  }
  inputFile.close();
  return filesImage;
}

std::vector<double>
ImageConverter::parseImageFileRow(const std::string &inputLine) {
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

void ImageConverter::loadKMatrixWithIdentifier(const std::string &tempId,
                                               const std::string &kMatrixId) {
  // if (kMatrices.count(kMatrixId)) {
  //   // Already loaded KMatrix with this id, don't need to load it again.
  //   return;
  // }

  // We are potentially loading the same KMatrix multiple times. It's a waste of
  // memory, but it isn't causing problems, so I'm just dealing with it
  // for now.

  boost::filesystem::directory_iterator end_itr;
  for (boost::filesystem::directory_iterator itr(kMatrixDirectory);
       itr != end_itr; ++itr) {
    Path pathToFile = itr->path();
    Path stemOfFile = pathToFile.stem();
    // If it's not a directory and the path contains
    if (is_regular_file(itr->path()) &&
        stemOfFile.string().find(kMatrixId) != std::string::npos) {
      kMatrices.insert(
          ImagePair(tempId, loadImageFromFile(pathToFile.string())));
    }
  }
}

void ImageConverter::loadTemperatureImagesWithIdentifier(
    const std::string &tempId) {

  Path temperatureDirectory(temperatureImagesDirectory);

  if (boost::filesystem::exists(temperatureDirectory) &&
      boost::filesystem::is_directory(temperatureDirectory)) {
    averageTemperatureImages.insert(ImagePair(
        tempId,
        getAndAverageImagesWithIdentifier(tempId, temperatureDirectory)));
  } else {
    throw std::runtime_error(
        "The temperature directory specified does not exist.");
  }
}

Image ImageConverter::getAndAverageImagesWithIdentifier(
    const std::string &identifier, const Path &path) {
  std::cout << "Loading images with identifier: " << identifier << std::endl;
  std::vector<Image> images;
  boost::filesystem::directory_iterator end_itr;
  for (boost::filesystem::directory_iterator itr(path); itr != end_itr; ++itr) {
    std::string pathToFile = itr->path().string();
    // If it's not a directory and the path contains id
    if (is_regular_file(itr->path()) &&
        pathToFile.find(identifier) != std::string::npos) {
      images.push_back(loadImageFromFile(pathToFile));
    }
  }
  return averageImages(images);
}

Image ImageConverter::averageImages(const std::vector<Image> &images) {
  if (images.size() == 0) {
    throw std::runtime_error(
        "Error! There were no images to load that match the specifier given.");
  }

  if (images.size() == 1) {
    return images.back();
  }

  Image resultantImage;
  for (int row = 0; row < images[0].size(); ++row) {
    std::vector<double> resultantImageRow;
    for (int column = 0; column < images[0].at(row).size(); ++column) {
      double sum = 0;
      for (auto &&image : images) {
        sum += image.at(row).at(column);
      }
      resultantImageRow.push_back(sum / images.size());
    }
    resultantImage.push_back(resultantImageRow);
  }
  return resultantImage;
}

std::pair<double, double> ImageConverter::loadAirTemperatures(
    double upperBeforeThermocouple, double upperAfterThermocouple,
    double lowerBeforeThermocouple, double lowerAfterThermocouple) {

  std::pair<double, double> tempPair;
  tempPair.first = (upperBeforeThermocouple + lowerBeforeThermocouple) / 2.0;
  tempPair.second = (upperAfterThermocouple + lowerAfterThermocouple) / 2.0;
  return tempPair;
}

// Convert from Excel coordinates to standard
Coordinate
ImageConverter::convertExcelNumberToStandard(const std::string &number) {
  // parse into ABC section (x direction) and 123 section (y direction).
  auto locationOfFirstNumber = number.find_first_of("1234567890");
  auto excelXCoordinate = number.substr(0, locationOfFirstNumber);
  auto excelYCoordinate = number.substr(locationOfFirstNumber);
  int rawXCoordinate =
      convertExcelXCoordinate(number.substr(0, locationOfFirstNumber));
  int rawYCoordinate = std::stoi(excelYCoordinate) - 1;

  return Coordinate(rawXCoordinate, rawYCoordinate);
}

int ImageConverter::convertExcelXCoordinate(
    const std::string &excelXCoordinate) {
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

///////////////////////////////////////////////////////////////////////////////
// Create conductance maps
// Creates and saves the conductance maps.

void ImageConverter::createConductanceMaps() {
  std::string fileName = baseSaveDirectory.generic_string() +
                         "ConductanceImages/" + date + "_Conductance_";
  Path dir(baseSaveDirectory.generic_string() + "ConductanceImages/");
  boost::filesystem::create_directory(dir);
  for (auto &&tempImagePair : averageTemperatureImages) {
    Image conductanceImage =
        createConductanceImage(tempImagePair.first, tempImagePair.second);
    Path fullFileName = (fileName + tempImagePair.first + ".csv");
    conductanceMaps.insert(
        std::make_pair(tempImagePair.first, conductanceImage));
    saveImage(fullFileName, conductanceImage);
  }
}

// Creates a particular conductance map.
Image ImageConverter::createConductanceImage(std::string imageIdentifier,
                                             const Image &tempImage) {
  Image conductanceImage;
  for (int row = 0; row < tempImage.size(); ++row) {
    std::vector<double> newRow;
    for (int column = 0; column < tempImage.at(row).size(); ++column) {
      newRow.push_back(calculateConductance(imageIdentifier, row, column,
                                            tempImage.at(row).at(column)));
      // std::cout << "(" << row << "," << column << ") , ";
    }
    conductanceImage.push_back(newRow);
    // std::cout << std::endl;
  }
  return conductanceImage;
}

// Calculates the conductance of a single pixel.
double ImageConverter::calculateConductance(std::string imageIdentifier,
                                            int row, int column,
                                            double pixelTemp) {

  // g = ( R + K(Ta - Tp) ) / ( Lw * (wp - wa) )
  const double Lw = 40.68;
  double K = getKMatrixValue(imageIdentifier, row, column);
  double Ta = getAirTemp(imageIdentifier, column);
  double Wa = getWaValue(imageIdentifier);
  double Wp = getWpValue(pixelTemp);

  double numerator = rValue + K * (Ta - pixelTemp);
  double denominator = Lw * (Wp - Wa);
  return numerator / denominator;
}

//////////////////////////////////////////////////////////////////////////////
double ImageConverter::getKMatrixValue(std::string imageIdentifier, int row,
                                       int column) {
  auto it = kMatrices.find(imageIdentifier);
  if (it != kMatrices.end()) {
    return it->second.at(row).at(column);
  } else {
    throw std::runtime_error("Temperature image " + imageIdentifier +
                             " does not have corresponding KMatrix.");
  }
}

// Gets the air temperature associated with a specific image number (from
// data
// file).
double ImageConverter::getAirTemp(std::string imageIdentifier, double column) {
  static double numberColumns =
      (averageTemperatureImages.begin()->second)[0].size();
  auto it = airTemps.find(imageIdentifier);
  if (it != airTemps.end()) {
    auto airTempPair = it->second;

    // airTempPair.first == airTemp at column 0
    // airTempPair.second == airTemp at column (numberColumns) /
    // (numberColumns)
    // say temperature is linear between them

    return airTempPair.second * (column / numberColumns) + airTempPair.first;
    // return airTempPair.first;
  } else {
    throw std::runtime_error("Temperature image " + imageIdentifier +
                             " does not have corresponding air temp value.");
  }
}

double ImageConverter::getAirTempGivenRatio(std::string imageId, double ratio) {
  auto it = airTemps.find(imageId);
  if (it != airTemps.end()) {
    auto airTempPair = it->second;

    // airTempPair.first == airTemp at column 0
    // airTempPair.second == airTemp at column (numberColumns) /
    // (numberColumns)
    // say temperature is linear between them

    return airTempPair.second * ratio + airTempPair.first;
    // return airTempPair.first;
  } else {
    throw std::runtime_error("Temperature image " + imageId +
                             " does not have corresponding air temp value.");
  }
}

// Get the wa value associated with a specific image number (from data
// file).
double ImageConverter::getWaValue(std::string imageIdentifier) {
  auto it = wa.find(imageIdentifier);
  if (it != wa.end()) {
    return it->second;
  } else {
    throw std::runtime_error("Temperature image " + imageIdentifier +
                             " does not have corresponding wa value.");
  }
}

// Gets the wp value of a pixel given its temperature.
double ImageConverter::getWpValue(double pixelTemp) {
  // w(p) = w0 * exp( -Tw / T(p))

  const double w0 = 6.57959 * pow(10, 8);
  const double Tw = 4982.85;

  return w0 * exp(-Tw / (pixelTemp + 273.15));
}

double ImageConverter::getDeltaWValue(const std::string &imageIdentifier,
                                      const Coordinate &coordinate) {
  double pixelTemp = getPixelTemp(imageIdentifier, coordinate);
  double deltaW = getWpValue(pixelTemp) - getWaValue(imageIdentifier);
  std::cout << deltaW << std::endl;
  return deltaW;
}

// Gets the temperature of a pixel given its image number and coordinate.
double ImageConverter::getPixelTemp(std::string imageIdentifier,
                                    const Coordinate &coordinate) {

  auto location = averageTemperatureImages.find(imageIdentifier);
  if (location != averageTemperatureImages.end()) {
    return (location->second).at(coordinate.second).at(coordinate.first);
  } else {
    throw std::runtime_error("Unexpected error saving leaflet data. "
                             "Conducatance number and temperature number do "
                             "not match.");
  }
}

// Gets the average temperature of a 9 pixel leaflet centered at the
// coordinate,
// and with image number specified.
double ImageConverter::getLeafletTemp(std::string imageIdentifier,
                                      const Coordinate &coordinate) {
  auto location = averageTemperatureImages.find(imageIdentifier);
  if (location != averageTemperatureImages.end()) {
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
    throw std::runtime_error(
        "Unexpected error saving leaflet data. "
        "Conductance identifier and temperature identifier do "
        "not match.");
  }
}

double ImageConverter::getLeafletAverageK(const std::string &imageKey,
                                          const Coordinate &coordinate) {
  auto location = kMatrices.find(imageKey);
  if (location != kMatrices.end()) {
    Image kMatrix = location->second;
    double sum = 0.0;
    sum += kMatrix.at(coordinate.second - 1).at(coordinate.first - 1);
    sum += kMatrix.at(coordinate.second - 1).at(coordinate.first);
    sum += kMatrix.at(coordinate.second - 1).at(coordinate.first + 1);
    sum += kMatrix.at(coordinate.second).at(coordinate.first - 1);
    sum += kMatrix.at(coordinate.second).at(coordinate.first);
    sum += kMatrix.at(coordinate.second).at(coordinate.first + 1);
    sum += kMatrix.at(coordinate.second + 1).at(coordinate.first - 1);
    sum += kMatrix.at(coordinate.second + 1).at(coordinate.first);
    sum += kMatrix.at(coordinate.second + 1).at(coordinate.first + 1);
    return sum / 9.0;
  } else {
    throw std::runtime_error("Unexpected error saving leaflet data. "
                             "KMatrix identifier and temperature identifier do "
                             "not match.");
  }
}

// Gets the average conductance of a 9 pixel leaflet centered at the
// coordinate,
// and with image number specified.
double ImageConverter::getLeafletConductance(const std::string &imageId,
                                             double leafletTemperature,
                                             const Coordinate &coordinate) {
  // g = ( R + K(Ta - Tp) ) / ( Lw * (wp - wa) )
  const double Lw = 40.68;
  double kValue = getLeafletAverageK(imageId, coordinate);
  double Wp = getWpValue(leafletTemperature);
  double Wa = getWaValue(imageId);
  double Ta = getAirTemp(imageId, coordinate.first);

  double numerator = rValue + kValue * (Ta - leafletTemperature);
  double denominator = Lw * (Wp - Wa);
  return numerator / denominator;

  return 0.0;
}

//////////////////////////////////////////////////////////////////////////////
void ImageConverter::saveAverageTemperatureImages() {
  std::ofstream outputFile;
  boost::filesystem::path dir(baseSaveDirectory.generic_string() +
                              "AverageTempImages/");
  boost::filesystem::create_directory(dir);

  std::string fileName = baseSaveDirectory.generic_string() +
                         "AverageTempImages/" + date + "_AverageTemp_";

  for (auto &&image : averageTemperatureImages) {
    Path fullPathToFile = Path(fileName + image.first + ".csv");
    saveImage(fullPathToFile, image.second);
  }
}

void ImageConverter::saveImage(const Path &fileName, const Image &image) {
  std::ofstream outputFile;
  outputFile.open(fileName.string());

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
    throw std::runtime_error("ERROR OPENING FILE: " + fileName.string());
    // std::cout << "ERROR OPENING FILE: " << fileName << std::endl;
  }
}

////////////////////////////////////////////////////////////////////////////////
/* FUNCTIONS DEALING WITH SAVING PIXEL DATA TO FILES */

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

// Creates the file that holds leaflet data, based on users preferences.
void ImageConverter::createSelectedPixelsFile(
    const std::vector<std::string> &coordinates) {
  std::ofstream outputFile;
  Path pathToFile = baseSaveDirectory.generic_string() + "PixelAnalysis.csv";
  outputFile.open(pathToFile.string());
  std::cout << "Saving file: " << pathToFile.string() << std::endl;
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
  outputFile << "Image identifier, Image Xout/Wa,, Pixel Temp , Pixel "
                "DeltaW , Pixel "
                "Conductance ,, Leaflet Temp, Leaflet DeltaW,  Leaflet "
                "Conductance"
             << std::endl;
}

// Prints the desired data (temp, conductance, delta w) for each
// pixel/leaflet.
void ImageConverter::printParticularPixelData(std::ofstream &outputFile,
                                              const Coordinate &coordinate) {
  // std::cout << "Printing coordinate: (" << coordinate.first << ","
  //           << coordinate.second << ")" << std::endl;
  for (auto &&temperatureImage : averageTemperatureImages) {
    std::string imageIdentifier = temperatureImage.first;

    // Print image identifier
    outputFile << imageIdentifier << ",";

    // Print image Wa value
    double waValue = getWaValue(imageIdentifier);
    outputFile << waValue << ",,";

    // Print pixel temp
    double pixelTemp = getPixelTemp(imageIdentifier, coordinate);
    outputFile << pixelTemp << ",";

    // Print pixel delta w
    outputFile << getWpValue(pixelTemp) - waValue << ",";

    // Print pixel conductance
    outputFile << calculateConductance(imageIdentifier, coordinate.second,
                                       coordinate.first, pixelTemp)
               << ",,";

    // Print leaflet temp
    double leafletTemp = getLeafletTemp(imageIdentifier, coordinate);
    outputFile << leafletTemp << ",";

    // Print leaflet delta w using average temperature
    outputFile << getWpValue(leafletTemp) - waValue << ",";

    // Print leaflet conductance using average temperature
    outputFile << getLeafletConductance(imageIdentifier, leafletTemp,
                                        coordinate)
               << std::endl;
  }
}

////////////////////////////////////////////////////////////////////////////////
/* Create K Matrix */

void ImageConverter::iterateThroughKMatrixDirectoriesAndCreate() {
  boost::filesystem::directory_iterator endItr;

  for (boost::filesystem::directory_iterator itr(kMatrixDirectory);
       itr != endItr; ++itr) {
    if (boost::filesystem::is_directory(itr->path())) {
      if (askIfKMatrixShouldBeCreated(itr->path())) {
        getKMatrixDirectoryInputs();
        createKMatrix(itr->path());
      }
    }
  }
}

bool ImageConverter::askIfKMatrixShouldBeCreated(const Path &path) {
  std::cout << "Would you like to make a KMatrix with the images in the path "
            << path << "?" << std::endl;
  return getYesNoResponseFromUser();
}

void ImageConverter::getKMatrixDirectoryInputs() {
  getRValueFromUser();

  double upperBeforeThermocouple =
      getTemperatureOfThermocouple("'upper before'");
  double upperAfterThermocouple = getTemperatureOfThermocouple("'upper after'");
  double lowerBeforeThermocouple =
      getTemperatureOfThermocouple("'lower before'");
  double lowerAfterThermocouple = getTemperatureOfThermocouple("'lower after'");

  std::pair<double, double> airTemp;
  airTemp.first = (upperBeforeThermocouple + lowerBeforeThermocouple) / 2.0;
  airTemp.second = (upperAfterThermocouple + lowerAfterThermocouple) / 2.0;

  airTemps.insert(std::make_pair("all", airTemp));
}

double ImageConverter::getTemperatureOfThermocouple(const std::string &name) {
  std::cout << "Please enter the temperature of the " << name << " thermocouple"
            << std::endl;
  std::string thermocoupleTemp;
  std::getline(std::cin, thermocoupleTemp);
  return std::stod(thermocoupleTemp);
}

void ImageConverter::createKMatrix(const Path &directory) {
  Image tempImage = loadAndAverageAllFilesInDirectory(directory);
  Image kMatrix;
  for (auto &&row : tempImage) {
    std::vector<double> kMatrixRow;
    for (int column = 0; column < row.size(); ++column) {
      kMatrixRow.push_back(getPixelKValue(row[column], column / row.size()));
    }
    kMatrix.push_back(kMatrixRow);
  }
  std::string fullPathName = kMatrixDirectory.generic_string() + "KMatrix_" +
                             directory.stem().generic_string() + ".csv";
  saveImage(Path(fullPathName), kMatrix);
}

Image ImageConverter::loadAndAverageAllFilesInDirectory(const Path &dir) {
  boost::filesystem::directory_iterator endItr;
  std::vector<Image> imagesInDirectory;

  for (boost::filesystem::directory_iterator itr(dir); itr != endItr; ++itr) {
    if (is_regular_file(itr->path()) &&
        itr->path().filename().string() != ".DS_Store") {
      imagesInDirectory.push_back(loadImageFromFile(itr->path()));
    }
  }
  return averageImages(imagesInDirectory);
}

double ImageConverter::getPixelKValue(double pixelTemp,
                                      double ratioOfColumnToNumColums) {
  // K(p) = R / (T(p) - T_air)
  double T_air = getAirTempGivenRatio("all", ratioOfColumnToNumColums);
  return rValue / (pixelTemp - T_air);
}
