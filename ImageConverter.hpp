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
  void loadNecessaryKMatrixFiles();
  void calculateKMatrix();
  void createMapOfKMatrices(std::map<int, Image> &);
  void averageKMatricesForFinal(const std::map<int, Image> &);
  Image getKMatrixFromTemperatureImage(double airTemp, const Image &);

  /* FUNCTIONS DEALING DIRECTLY WITH CREATING CONDUCTANCE MAPS */
  void runConductanceMapCreationProgram();
  void loadNecessaryConductanceProgramFiles();
  void calculateConductanceMaps();
  void createConductanceMaps();
  Image createConductanceImage(int imageNumber, const Image &);
  double calculateConductance(int, int, int, double);
  void summarizeSelectedPixels();

  /* FUNCTIONS DEALING WITH PATCH DATA CALCULATION PROGRAM EXECUTION */
  void runPixelSummaryProgram();
  void readAverageTemperatureImages();
  void readConductanceImages();

  /* FUNCTIONS DEALING WITH LOADING DATA FROM FILES */
  void loadKMatrix();
  void loadDataFile();
  void parseInputFileLine(std::istringstream &);
  void loadTemperatureData();
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
  void checkDataAndImageNumberCompatability();
  bool dataFileNumberHasMatchingTempFile(int);

  /* FUNCTIONS DEALING WITH AVERING A SERIES OF IMAGES */
  void averageRawTemperatureImages();
  void
  averageImages(int,
                std::pair<ImageMultimap::iterator, ImageMultimap::iterator>);

  /* FUNCTIONS DEALING WITH CONVERTING FROM EXCEL COORDINATES TO STANDARD */
  Coordinate convertExcelNumberToStandard(std::string);
  int convertExcelXCoordinate(std::string);
  int getCharValue(char);

  /* FUNCTIONS DEALING WITH GETTING NECESSARY PARAMETERS FOR EQUATIONS */
  double getAirTemp(int);
  double getWaValue(int);
  double getWpValue(double);
  double getPixelTemp(int, const Coordinate &);
  double getLeafletTemp(int, const Coordinate &);
  double getLeafletConductance(const Image &, const Coordinate &);
};

#endif
