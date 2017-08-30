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
  int rValue;

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

  // The first in the pair is the average air temperature on the left side of
  // the chamber, and the second in the pair is on the right side of the
  // chamber.
  std::map<std::string, std::pair<double, double>> airTemps;
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
  void getRValueFromUser();
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
  void parseInputFileLine(std::istringstream &);
  Image loadImageFromFile(const Path &);
  std::vector<double> parseImageFileRow(const std::string &);
  void loadKMatrixWithIdentifier(const std::string &tempId,
                                 const std::string &kMatrixId);
  void loadTemperatureImagesWithIdentifier(const std::string &tempId);
  Image getAndAverageImagesWithIdentifier(const std::string &, const Path &);
  Image averageImages(const std::vector<Image> &);
  std::pair<double, double> loadAirTemperatures(double flThermo,
                                                double blThermo,
                                                double frThermo,
                                                double brThermo);

  // Convert from Excel coordinates to standard
  Coordinate convertExcelNumberToStandard(const std::string &);
  int convertExcelXCoordinate(const std::string &);
  int getCharValue(char);

  // Create conductance maps
  void createConductanceMaps();
  Image createConductanceImage(std::string, const Image &);
  double calculateConductance(std::string, int, int, double);

  // Get data for conductance equations
  double getKMatrixValue(std::string, int, int);
  double getAirTemp(std::string, double);
  double getWaValue(std::string);
  double getWpValue(double);
  double getDeltaWValue(const std::string &, int, int);
  double getPixelTemp(std::string, const Coordinate &);
  double getLeafletTemp(std::string, const Coordinate &);
  double getLeafletConductance(const Image &, const Coordinate &);
  double getLeafletDeltaW(const std::string &, const Coordinate &);

  // Save data to files
  void saveAverageTemperatureImages();
  void saveImage(const Path &, const Image &);
};

#endif
