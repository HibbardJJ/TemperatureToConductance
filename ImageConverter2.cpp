#include "ImageConverter2.hpp"
#include <iostream>

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
  case 3:
    runPixelSummaryProgram(pathToBaseDirectory);
    break;
  }
}

////////////////////////////////////////////////////////////////////////////////
/* MAIN PROGRAM EXECUTION */

void ImageConverter::runKMatrixCreationProgram(
    const Path &pathToBaseDirectory) {
  std::cout << "Starting KMatrix Creation Program" << std::endl;
  initializeVariablesForKMatrixProgram(pathToBaseDirectory);
}

void ImageConverter::runConductanceMapCreationProgram(
    const Path &pathToBaseDirectory) {
  std::cout << "Starting Conductance Map Creation Program" << std::endl;
  initializeVariablesForConductanceMapProgram(pathToBaseDirectory);
  confirmConductanceMapVariableInitializationIsCorrect();
}

void ImageConverter::runPixelSummaryProgram(const Path &pathToBaseDirectory) {
  std::cout << "Starting Pixel Summary Creation Program" << std::endl;
  initializeVariablesForPixelSummaryProgram(pathToBaseDirectory);
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
  temperatureImagesDirectory =
      Path(basePath + "Data/" + date + "/TemperatureImages/");
  kMatrixDirectory = Path(basePath + "KMatrix/");
  topLeftWindowCoordinate = convertExcelNumberToStandard("EX72");
  bottomRightWindowCoordinate = convertExcelNumberToStandard("VN434");
}

/* Pixel Calculation Program */
void ImageConverter::initializeVariablesForPixelSummaryProgram(
    const Path &pathToBaseDirectory) {
  std::string basePath = pathToBaseDirectory.generic_string();
  getDateFromUser();
  baseSaveDirectory = Path(basePath + "Data/" + date + "/");
  programDataInputFile =
      Path(basePath + "Data/" + date + "/DataExtraction.csv");
  temperatureImagesDirectory =
      Path(basePath + "Data/" + date + "/AverageTempImages/");
  kMatrixDirectory = Path(basePath + "KMatrix/"); // Shouldn't be used
  topLeftWindowCoordinate = Coordinate(0, 0);
  bottomRightWindowCoordinate = Coordinate(1000000, 1000000);
}

////////////////////////////////////////////////////////////////////////////////
/* CONFIRM INITIALIZED VARIABLES ARE CORRECT */

void ImageConverter::confirmConductanceMapVariableInitializationIsCorrect() {
  confirmBaseSaveDirectoryPathIsCorrect();
  confirmKMatrixDirectoryPathIsCorrect();
  confirmProgramDataInputFilePathIsCorrect();
  confirmTemperatureFilesPathIsCorrect();
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
  std::cout << "Is the correct path for the " << message << path << "?"
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

////////////////////////////////////////////////////////////////////////////////
/* BASIC USER INPUT COMMUNICATION */

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

void ImageConverter::getDateFromUser() {
  std::cout << "Please enter the date of the data used (YYYY-MM-DD)."
            << std::endl;
  std::getline(std::cin, date);
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
