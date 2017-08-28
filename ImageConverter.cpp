#include "ImageConverter.hpp"
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
  kMatrixLocation = baseWorkingDirectory + "KMatrix/";

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
 *    Temperature Specifier | KMatrix Specifier | LF Thermocouple | LB
 *    Thermocouple | RF Thermocouple | RB Thermocouple | Wa
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
  // std::cout << "Creating K Matrix..." << std::endl;
  getDate();
  boost::filesystem::path path(baseWorkingDirectory + "KMatrix/" + date);
  boost::filesystem::directory_iterator end_itr;
  for (boost::filesystem::directory_iterator itr(path); itr != end_itr; ++itr) {
    // If it's not a directory and the path contains
    if (boost::filesystem::is_directory(itr->path()) &&
        askIfKMatrixShouldBeCreated(itr->path())) {
      createKMatrixFromImagesInPath(itr->path());
    }
  }
}

bool ImageConverter::askIfKMatrixShouldBeCreated(boost::filesystem::path path) {
  std::cout
      << "Would you like to create a KMatrix from the files in the directory \""
      << path << "\"?" << std::endl;
  return getYesNoResponseFromUser();
}

void ImageConverter::createKMatrixFromImagesInPath(
    boost::filesystem::path path) {}

/* Loads necessary files to create a KMatrix */
void ImageConverter::loadNecessaryKMatrixFiles() {
  loadDataFile();
  readTemperatureImagesFromDirectory(baseSaveDirectory + "AverageTempImages/");
}

/* Runs the necessary functions to calculate the KMatrix and save it to the
member variable kMatrix. */
void ImageConverter::calculateKMatrix() {
  // checkDataAndImageNumberCompatability();
  // averageRawTemperatureImages();
  // saveAveragedTemperatureImages();
  // std::map<std::string, Image> kMatrices;
  // createMapOfKMatrices(kMatrices);
  // averageKMatricesForFinal(kMatrices);
}

/* Creates a map of KMatrices given different input temperature images groups */
// void ImageConverter::createMapOfKMatrices(
//     std::map<std::string, Image> &kMatrices) {
//   for (auto &&imagePair : averagedTemperatureImages) {
//     double airTemp = getAirTemp(imagePair.first);
//     Image kMatrix = getKMatrixFromTemperatureImage(airTemp,
//     imagePair.second);
//     kMatrices.insert(std::make_pair(imagePair.first, kMatrix));
//   }
// }

/* Averages all the different KMatrix solutions for different temperature input
 * groups */
// void ImageConverter::averageKMatricesForFinal(
//     const std::map<std::string, Image> &kMatrices) {
//   for (int row = 0; row < kMatrices.begin()->second.size(); ++row) {
//     std::vector<double> kMatrixRow;
//     for (int column = 0; column < kMatrices.begin()->second[0].size();
//          ++column) {
//       double sum = 0;
//       for (auto &&matrixPair : kMatrices) {
//         sum += matrixPair.second[row][column];
//       }
//       double average = sum / kMatrices.size();
//       kMatrixRow.push_back(average);
//     }
//     kMatrix.push_back(kMatrixRow);
//   }
// }

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
  saveAveragedTemperatureImages();
  createConductanceMaps();
  summarizeSelectedPixels();
}

// Loads files necessary to create conductance maps
void ImageConverter::loadNecessaryConductanceProgramFiles() {
  std::cout << "Loading necessary files. This could take a while..."
            << std::endl;
  // loadKMatrix();
  loadDataFile();
  readTemperatureImagesFromDirectory(rawTemperatureDirectory);
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
    std::string fullFileName = fileName + tempImagePair.first + ".csv";
    conductanceImages.insert(
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
    }
    conductanceImage.push_back(newRow);
  }
  return conductanceImage;
}

// Calculates the conductance of a single pixel.
double ImageConverter::calculateConductance(std::string imageIdentifier,
                                            int row, int column,
                                            double pixelTemp) {

  // g = ( R + K(Ta - Tp) ) / ( Lw * (wp - wa) )
  const double Lw = 40.68;
  double K = getKMatrix(imageIdentifier).at(row).at(column);
  double Ta = getAirTemp(imageIdentifier);
  double Wa = getWaValue(imageIdentifier);
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
  bottomRightWindowCoordinate = Coordinate(1000000, 1000000);
  getDate();
  getBaseDir();
  getProgramInputFile();
  loadDataFile();
  readTemperatureImagesFromDirectory(baseSaveDirectory + "AverageTempImages/");
  readConductanceImagesFromDirectory();
  std::vector<std::string> coordinatesToAnalyze = getPixelChoicesFromUser();
  createSelectedPixelsFile(coordinatesToAnalyze);
}

void ImageConverter::readTemperatureImagesFromDirectory(
    const std::string &directoryPath) {
  boost::filesystem::path temperatureDirectory(directoryPath);

  if (boost::filesystem::exists(temperatureDirectory) &&
      boost::filesystem::is_directory(temperatureDirectory)) {
    for (auto &&identifierTempPair : airTemp) {
      averagedTemperatureImages.insert(getFilesFromDirectoryWithIdentifier(
          temperatureDirectory, identifierTempPair.first));
    }
  }
}

void ImageConverter::readConductanceImagesFromDirectory() {
  boost::filesystem::path conductanceDirectory(baseSaveDirectory +
                                               "ConductanceImages/");
  if (boost::filesystem::exists(conductanceDirectory) &&
      boost::filesystem::is_directory(conductanceDirectory)) {
    for (auto &&identifierTempPair : airTemp) {
      conductanceImages.insert(getFilesFromDirectoryWithIdentifier(
          conductanceDirectory, identifierTempPair.first));
    }
  }
}

std::pair<std::string, Image>
ImageConverter::getFilesFromDirectoryWithIdentifier(
    boost::filesystem::path path, std::string identifier) {
  std::vector<Image> images;
  boost::filesystem::directory_iterator end_itr;
  for (boost::filesystem::directory_iterator itr(path); itr != end_itr; ++itr) {
    std::string pathToFile = itr->path().string();
    // If it's not a directory and the path contains
    if (is_regular_file(itr->path()) &&
        pathToFile.find(identifier) != std::string::npos) {
      images.push_back(loadImageFromFile(pathToFile));
    }
  }
  return std::pair<std::string, Image>(identifier, getAverageOfImages(images));
}

Image ImageConverter::getAverageOfImages(const std::vector<Image> &images) {
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

////////////////////////////////////////////////////////////////////////////////
/* FUNCTIONS DEALING WITH LOADING DATA FROM FILES */

// Loads KMatrix to local variable kMatrix.
void ImageConverter::loadKMatrix(std::string tempId, std::string kMatrixId) {
  boost::filesystem::directory_iterator end_itr;
  for (boost::filesystem::directory_iterator itr(kMatrixLocation);
       itr != end_itr; ++itr) {
    std::string pathToFile = itr->path().string();
    if (is_regular_file(itr->path()) &&
        pathToFile.find(kMatrixId) != std::string::npos) {
      Image kMatrix = loadImageFromFile(pathToFile);
      kMatrices.insert(std::pair<std::string, Image>(tempId, kMatrix));
      break;
    }
  }
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

  // Read temperature image identifier
  std::getline(rowToParse, data, ',');
  std::string imageIdentifier = data;

  // Read kMatrix image identifier
  std::getline(rowToParse, data, ',');
  loadKMatrix(imageIdentifier, data);

  // Read air temperature;
  std::getline(rowToParse, data, ',');
  double rowsAirTemp = std::stod(data);
  airTemp.insert(std::make_pair(imageIdentifier, rowsAirTemp));

  // Read Wa
  std::getline(rowToParse, data, ',');
  double rowsWa = std::stod(data);
  wa.insert(std::make_pair(imageIdentifier, rowsWa));
}

// Double checks that the file should be loaded (its number is specified in
// data
// file).
bool ImageConverter::confirmShouldLoadImageBasedOnDataFile(
    std::string imageNumber) {
  if (airTemp.count(imageNumber)) {
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
    std::string fullName = fileName + image.first + ".csv";
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
  std::cout << "Printing coordinate: (" << coordinate.first << ","
            << coordinate.second << ")" << std::endl;
  for (auto &&conductanceImage : conductanceImages) {
    std::string imageIdentifier = conductanceImage.first;

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
    outputFile
        << (conductanceImage.second).at(coordinate.second).at(coordinate.first)
        << ",,";

    // Alternative
    // outputFile << calculateConductance(imageIdentifier,
    // coordinate.second,
    //                                    coordinate.first, pixelTemp)
    //            << ",";

    // Print leaflet temp
    double leafletTemp = getLeafletTemp(imageIdentifier, coordinate);
    outputFile << leafletTemp << ",";

    // Print leaflet delta w: gets delta w of each pixel and averages
    outputFile << getLeafletDeltaW(imageIdentifier, coordinate) << ",";
    // outputFile << getWpValue(leafletTemp) - waValue << ",";

    // Print leaflet conductance: gets conductance of each pixel and
    // averages
    outputFile << getLeafletConductance(conductanceImage.second, coordinate)
               << std::endl;
  }
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

// Converts just the x coordinate (ABC..) of an excel coordinate to
// standard.
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

Image ImageConverter::getKMatrix(std::string imageIdentifier) {
  auto it = kMatrices.find(imageIdentifier);
  if (it != kMatrices.end()) {
    return it->second;
  } else {
    throw std::runtime_error("Temperature image " + imageIdentifier +
                             " does not have corresponding KMatrix.");
  }
}

// Gets the air temperature associated with a specific image number (from
// data
// file).
double ImageConverter::getAirTemp(std::string imageIdentifier) {
  auto it = airTemp.find(imageIdentifier);
  if (it != airTemp.end()) {
    return it->second;
  } else {
    throw std::runtime_error("Temperature image " + imageIdentifier +
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
                                      int row, int column) {
  double pixelTemp = getPixelTemp(imageIdentifier, Coordinate(column, row));
  double deltaW = getWpValue(pixelTemp) - getWaValue(imageIdentifier);
  std::cout << deltaW << std::endl;
  return deltaW;
}

// Gets the temperature of a pixel given its image number and coordinate.
double ImageConverter::getPixelTemp(std::string imageIdentifier,
                                    const Coordinate &coordinate) {

  auto location = averagedTemperatureImages.find(imageIdentifier);
  if (location != averagedTemperatureImages.end()) {
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
  auto location = averagedTemperatureImages.find(imageIdentifier);
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

// Gets the average conductance of a 9 pixel leaflet centered at the
// coordinate,
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

double ImageConverter::getLeafletDeltaW(const std::string &identifier,
                                        const Coordinate &coordinate) {
  double sum = 0.0;
  sum +=
      getDeltaWValue(identifier, coordinate.second - 1, coordinate.first - 1);
  sum += getDeltaWValue(identifier, coordinate.second - 1, coordinate.first);
  sum +=
      getDeltaWValue(identifier, coordinate.second - 1, coordinate.first + 1);
  sum += getDeltaWValue(identifier, coordinate.second, coordinate.first - 1);
  sum += getDeltaWValue(identifier, coordinate.second, coordinate.first);
  sum += getDeltaWValue(identifier, coordinate.second, coordinate.first + 1);
  sum +=
      getDeltaWValue(identifier, coordinate.second + 1, coordinate.first - 1);
  sum += getDeltaWValue(identifier, coordinate.second + 1, coordinate.first);
  sum +=
      getDeltaWValue(identifier, coordinate.second + 1, coordinate.first + 1);
  return sum / 9.0;
}
