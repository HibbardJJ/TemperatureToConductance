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

class ImageConverter {
public:
  ImageConverter(std::string baseDirectory);
  void getUserInputs();
  void loadNecessaryFiles();
  void calculateConductanceMaps();
  void summarizeSelectedPixels();

private:
  std::string date;
  std::string baseWorkingDirectory;
  std::string baseSaveDirectory;
  std::string rawTemperatureDirectory;
  std::string kMatrixLocation;
  std::string programInputFile;

  Image kMatrix;
  int rValue;
  ImageMultimap rawTemperatureImages;
  std::map<int, Image> averagedTemperatureImages;
  std::map<int, double> airTemp;
  std::map<int, double> wa;
  std::map<int, Image> conductanceImages;

  // Functions dealing with user input
  bool checkCorrectLocation(std::string, std::string);
  std::string getCorrectLocation(std::string);
  void getDate();
  void getRawTemperatureDir();
  void getBaseDir();
  void getKMatrix();
  void getRValue();
  void getProgramInputFile();

  // Functions dealing with loading data from files
  void loadKMatrix();
  void loadDataFile();
  void loadTemperatureData();
  Image loadImageFromFile(std::string fileName);
  void parseInputFileLine(std::istringstream &);

  // Functions dealing with converting data to conductance
  void checkDataAndImageNumberCompatability();
  bool dataFileNumberHasMatchingTempFile(int);
  void saveImage(const std::string &fileName, const QImage &image);

  // Fuctions dealing with the averaging of temperature images
  void averageRawTemperatureImages();
  void
  averageImages(int,
                std::pair<ImageMultimap::iterator, ImageMultimap::iterator>);
  void saveAveragedTemperatureImages();

  // std::vector<int> getImageNumbers();
  void createConductanceMaps();
  void saveConductanceMaps();

  // Functions dealing with saving off selected pixels data
  void getPixelChoicesFromUser();
  void createFile();
};

#endif
