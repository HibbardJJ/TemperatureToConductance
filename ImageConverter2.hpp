#ifndef IMAGE_CONVERTER
#define IMAGE_CONVERTER

#include <boost/filesystem.hpp>
#include <map>
#include <string>
#include <vector>

using Path = boost::filesystem::path;
using Image = std::vector<std::vector<double>>;
using ImageMap = std::map<std::string, Image>;
using ImagePair = std::pair<std::string, Image>;
using Coordinate = std::pair<int, int>;

class ImageConverter {
public:
  ImageConverter(const Path &);

private:
  std::string date;

  // Holds the paths to important directories/files needed in program.
  Path baseSaveDirectory;
  Path programDataInputFile;
  Path temperatureImagesDirectory;
  Path kMatrixDirectory;

  // Coordinates needed to crop raw temperature images to correct window size
  Coordinate topLeftWindowCoordinate;
  Coordinate bottomRightWindowCoordinate;

  // Maps of data needed in program.
  // The key of the map is the image identifier
  ImageMap kMatrices;
  ImageMap averageTemperatureImages;
  ImageMap conductanceMaps;
  ImageMap airTemp; // TODO!!! Make only one row thick
  std::map<std::string, double> wa;

  // Main Program Execution
  void runKMatrixCreationProgram(const Path &);
  void runConductanceMapCreationProgram(const Path &);
  void runPixelSummaryProgram(const Path &);

  // Initialize variables particular to each program execution type.
  void initializeVariablesForKMatrixProgram(const Path &);
  void initializeVariablesForConductanceMapProgram(const Path &);
  void initializeVariablesForPixelSummaryProgram(const Path &);

  // Basic user input communication.
  int getProgramExecutionType();
  void getDateFromUser();
  bool getYesNoResponseFromUser();

  // Confirm preinitalized variables are correct.
  void confirmConductanceMapVariableInitializationIsCorrect();
  void confirmBaseSaveDirectoryPathIsCorrect();
  void confirmKMatrixDirectoryPathIsCorrect();
  void confirmProgramDataInputFilePathIsCorrect();
  void confirmTemperatureFilesPathIsCorrect();
  bool askIfPathIsCorrectForFile(const std::string &message, const Path &path);
  Path getCorrectPathFromUser();

  // Load necessary data
  void loadAllConductanceProgramData();
  void parseInputFileLine();
  void loadImageFromFile(const Path &);
  void loadKMatrixWithIdentifier(const std::string &tempId,
                                 const std::string &kMatrixId);
  void loadTemperatureImageWithIdentifier(const std::string &tempId);
  void createAirTemperatureMatrix(double flThermo, double blThermo,
                                  double frThermo, double brThermo);

  // Convert from Excel coordinates to standard
  Coordinate convertExcelNumberToStandard(std::string);
  int convertExcelXCoordinate(std::string);
  int getCharValue(char);
};

#endif
