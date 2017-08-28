/*
  Image Converter class
  Author: Katie Sweet
*/

#ifndef IMAGE_CONVERTER
#define IMAGE_CONVERTER

#include <boost/filesystem.hpp>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using Image = std::vector<std::vector<double>>;
using ImageMultimap = std::multimap<std::string, Image>;
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

  std::map<std::string, Image> kMatrices;
  int rValue;
  std::map<std::string, Image> averagedTemperatureImages;
  std::map<std::string, double> airTemp;
  std::map<std::string, double> wa;
  std::map<std::string, Image> conductanceImages;

  /* GENERIC USER INPUT */
  int getProgramExecutionType();
  void getDate();
  void getRawTemperatureDir();
  void getBaseDir();
  void getKMatrix();
  void getRValue();
  void getProgramInputFile();
  void getWindowCoordinates();
  std::vector<std::string> getPixelChoicesFromUser();
  bool checkCorrectLocation(std::string, std::string);
  std::string getCorrectLocation(std::string);
  bool getYesNoResponseFromUser();
  void getUserInputs();

  /* FUNCTIONS DEALING DIRECTLY WITH CREATING A K MATRIX */
  void runKMatrixCreationProgram();
  bool askIfKMatrixShouldBeCreated(boost::filesystem::path);
  void createKMatrixFromImagesInPath(boost::filesystem::path);
  void loadNecessaryKMatrixFiles();
  void calculateKMatrix();
  // void createMapOfKMatrices(std::map<std::string, Image> &);
  // void averageKMatricesForFinal(const std::map<std::string, Image> &);
  // Image getKMatrixFromTemperatureImage(double airTemp, const Image &);

  /* FUNCTIONS DEALING DIRECTLY WITH CREATING CONDUCTANCE MAPS */
  void runConductanceMapCreationProgram();
  void loadNecessaryConductanceProgramFiles();
  void createConductanceMaps();
  Image createConductanceImage(std::string, const Image &);
  double calculateConductance(std::string, int, int, double);
  void summarizeSelectedPixels();

  /* FUNCTIONS DEALING WITH PATCH DATA CALCULATION PROGRAM EXECUTION */
  void runPixelSummaryProgram();
  // void readConductanceImages();

  /* FUNCTIONS DEALING WITH LOADING DATA FROM FILES */

  void loadKMatrix(std::string, std::string);

  void loadDataFile();
  void parseInputFileLine(std::istringstream &);
  void readTemperatureImagesFromDirectory(const std::string &);
  void readConductanceImagesFromDirectory();
  std::pair<std::string, Image>
      getFilesFromDirectoryWithIdentifier(boost::filesystem::path, std::string);
  bool confirmShouldLoadImageBasedOnDataFile(std::string);
  Image loadImageFromFile(std::string fileName);
  std::vector<double> parseRow(std::string &);

  /* FUNCTIONS DEALING WITH SAVING IMAGES TO FILES */
  void saveImage(const std::string &fileName, const Image &image);
  void saveAveragedTemperatureImages();

  /* FUNCTIONS DEALING WITH SAVING PIXEL DATA TO FILES */
  void createSelectedPixelsFile(const std::vector<std::string> &);
  void writeCoordinateHeader(std::ofstream &, const Coordinate &);
  void printParticularPixelData(std::ofstream &, const Coordinate &);

  /* FUNCTIONS DEALING WITH DATA INPUT COMPATIBILITY */
  // void checkDataAndImageNumberCompatability();
  // bool dataFileNumberHasMatchingTempFile(std::string);

  /* FUNCTIONS DEALING WITH AVERING A SERIES OF IMAGES */
  // void averageRawTemperatureImages();
  // void averageImages(
  //     std::string, std::pair<ImageMultimap::iterator,
  //     ImageMultimap::iterator>);
  Image getAverageOfImages(const std::vector<Image> &);

  /* FUNCTIONS DEALING WITH CONVERTING FROM EXCEL COORDINATES TO STANDARD */
  Coordinate convertExcelNumberToStandard(std::string);
  int convertExcelXCoordinate(std::string);
  int getCharValue(char);

  /* FUNCTIONS DEALING WITH GETTING NECESSARY PARAMETERS FOR EQUATIONS */
  Image getKMatrix(std::string);
  double getAirTemp(std::string);
  double getWaValue(std::string);
  double getWpValue(double);
  double getDeltaWValue(const std::string &, int, int);
  double getPixelTemp(std::string, const Coordinate &);
  double getLeafletTemp(std::string, const Coordinate &);
  double getLeafletConductance(const Image &, const Coordinate &);
  double getLeafletDeltaW(const std::string &, const Coordinate &);
};

#endif
