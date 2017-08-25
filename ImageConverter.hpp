/*
  Image Converter class
  Author: Katie Sweet
*/

#ifndef IMAGE_CONVERTER
#define IMAGE_CONVERTER

#include <map>
#include <sstream>
#include <string>
#include <vector>

using Image = std::vector<std::vector<double>>;
using ImageMultimap = std::multimap<int, Image>;
using Coordinate = std::pair<int, int>;

class ImageConverter {
public:
  ImageConverter(std::string);
  void chooseProgramTypeAndExecute();

private:
  std::string date;
  std::string baseWorkingDirectory;
  std::string baseSaveDirectory;
  std::string rawTemperatureDirectory;
  std::string kMatrixLocation;
  std::string programInputFile;

  Coordinate topLeftWindowCoordinate;
  Coordinate bottomRightWindowCoordinate;

  Image kMatrix;
  int rValue;
  ImageMultimap rawTemperatureImages;
  std::map<int, Image> averagedTemperatureImages;
  std::map<int, double> airTemp;
  std::map<int, double> wa;
  std::map<int, Image> conductanceImages;

  // Main program input selection
  int getProgramExecutionType();

  // Main Conductance Calculation Program Execution
  void runConductanceMapCreationProgram();
  void getUserInputs();
  void loadNecessaryFiles();
  void calculateConductanceMaps();
  void summarizeSelectedPixels();

  // Main KMatrix Calculation Program Execution
  void runKMatrixCreationProgram();
  void loadNecessaryKMatrixFiles();
  void calculateKMatrix();
  void createMapOfKMatrices(std::map<int, Image> &);
  void averageKMatricesForFinal(const std::map<int, Image> &);
  Image getKMatrixFromTemperatureImage(double airTemp, const Image &);

  // Main Pixel Calculation Program Execution
  void runPixelSummaryProgram();
  void readAverageTemperatureImages();
  void readConductanceImages();

  // Functions dealing with user input
  bool checkCorrectLocation(std::string, std::string);
  std::string getCorrectLocation(std::string);
  void getDate();
  void getRawTemperatureDir();
  void getBaseDir();
  void getKMatrix();
  void getRValue();
  void getProgramInputFile();
  void getWindowCoordinates();
  bool getYesNoResponseFromUser();

  // Functions dealing with loading data from files
  void loadKMatrix();
  void loadDataFile();
  void loadTemperatureData();
  // std::pair<int, Image> loadImageFile(std::string);
  bool confirmShouldLoadImageBasedOnDataFile(std::string);
  Image loadImageFromFile(std::string fileName);
  void parseInputFileLine(std::istringstream &);
  std::vector<double> parseRow(std::string &);

  // Functions dealing with converting data to conductance
  void checkDataAndImageNumberCompatability();
  bool dataFileNumberHasMatchingTempFile(int);
  void saveImage(const std::string &fileName, const Image &image);

  // Fuctions dealing with the averaging of temperature images
  void averageRawTemperatureImages();
  void
  averageImages(int,
                std::pair<ImageMultimap::iterator, ImageMultimap::iterator>);
  void saveAveragedTemperatureImages();

  // std::vector<int> getImageNumbers();
  void createConductanceMaps();
  Image createConductanceImage(int imageNumber, const Image &);
  double calculateConductance(int, int, int, double);
  double getAirTemp(int);
  double getWaValue(int);
  double getWpValue(double);

  // Functions dealing with converting from Excel coordinates to Standard
  Coordinate convertExcelNumberToStandard(std::string);
  int convertExcelXCoordinate(std::string);
  int getCharValue(char);

  // Functions dealing with saving off selected pixels data
  std::vector<std::string> getPixelChoicesFromUser();
  void createSelectedPixelsFile(const std::vector<std::string> &);
  void writeCoordinateHeader(std::ofstream &, const Coordinate &);
  void printParticularPixelData(std::ofstream &, const Coordinate &);
  double getPixelTemp(int, const Coordinate &);
  double getLeafletTemp(int, const Coordinate &);
  double getLeafletConductance(const Image &, const Coordinate &);
};

#endif
