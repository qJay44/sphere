#include "shapefile.hpp"
#include "utils/utils.hpp"

#include <bit>
#include <cstring>
#include <fstream>
#include <ios>
#include <type_traits>

using namespace shapefile;

namespace {

template<std::integral T>
constexpr T swapBytes(T val) {
  if constexpr (sizeof(T) == 4)
    return __builtin_bswap32(val);
  else if constexpr (sizeof(T) == 8)
    return __builtin_bswap64(val);

  return val;
}

template<typename T>
[[nodiscard]] T readValue(const char* buffer, std::endian byteOrder) {
  T result;
  std::memcpy(&result, buffer, sizeof(T));

  if (std::endian::native != byteOrder) {
    if constexpr (std::is_integral_v<T>) {
      return swapBytes(result);
    } else if (std::is_floating_point_v<T>) {
      using IntType = std::conditional_t<sizeof(T) == 4, u32, u64>; // Just use unsigned for safety
      auto intVal = std::bit_cast<IntType>(result);
      return std::bit_cast<T>(intVal);
    }
  }

  return result;
}

const fspath rootFolder = "res/shapefile";

}

s32 ShapefileReader::toInteger(const char* buffer, std::endian byteOrder) {
  return readValue<s32>(buffer, byteOrder);
}

double ShapefileReader::toDouble(const char* buffer, std::endian byteOrder) {
  return readValue<double>(buffer, byteOrder);
}

ShapefileReader::ShapefileReader(const fspath& folderName, bool printInfo) {
  std::string namePrefix = (rootFolder / folderName.string() / folderName.string()).string();

  std::ifstream file(namePrefix + ".shp");
  if (!file.is_open())
    error("[ShapefileReader::ShapefileReader] Can't open file [{}]", folderName.string());

  file.seekg(0, std::ios::end);
  size_t length = file.tellg();
  file.seekg(0, std::ios::beg);

  buffer = new char[length];
  file.read(buffer, length);

  // Byte 0
  fileCode = toInteger(buffer, std::endian::big);
  if (fileCode != 9994)
    error("[ShapefileReader] file code is not 9994");

  // Bytes 4 - 23 Unused

  // Byte 24
  fileLength = toInteger(buffer + 24, std::endian::big);

  // Byte 28
  version = toInteger(buffer + 28, std::endian::little);
  if (version != 1000)
    error("[ShapefileReader] version is not 1000");

  // Byte 32
  type = (shapefile_t)toInteger(buffer + 32, std::endian::little);

  bb.Xmin = toDouble(buffer + 36, std::endian::little); // Byte 36
  bb.Ymin = toDouble(buffer + 44, std::endian::little); // Byte 44
  bb.Xmax = toDouble(buffer + 52, std::endian::little); // Byte 52
  bb.Ymax = toDouble(buffer + 60, std::endian::little); // Byte 60
  bb.Zmin = toDouble(buffer + 68, std::endian::little); // Byte 68
  bb.Zmax = toDouble(buffer + 76, std::endian::little); // Byte 76
  bb.Mmin = toDouble(buffer + 84, std::endian::little); // Byte 84
  bb.Mmax = toDouble(buffer + 92, std::endian::little); // Byte 92

  if (printInfo) {
    puts("\n==================== [ShapeReader] Main file header ====================\n");
    printf("File code: %d\n", fileCode);
    printf("File length: %d\n", fileLength);
    printf("Version: %d\n", version);
    printf("Shape type: %d\n", (s32)type);
    bb.print();
    puts("\n========================================================================\n");
  }

  if (bb.empty())
    warning("[ShapefileReader] has no records");

  file.close();
}

ShapefileReader::~ShapefileReader() {
  delete[] buffer;
}

const s32& ShapefileReader::getLength() const {
  return fileLength;
}

const shapefile_t& ShapefileReader::getType() const {
  return type;
}

const char* ShapefileReader::getFirstRecordPtr() const {
  return buffer + mainFileHeaderSizeInBytes;
}

s32 ShapefileReader::getRecordsSizeInBytes() const {
  return getLength() * 2 - mainFileHeaderSizeInBytes;
}

bool ShapefileReader::BoundingBox::empty() const {
  return (
    Xmin == 0.0 && Ymin == 0.0 &&
    Xmax == 0.0 && Ymax == 0.0
  );
}

void ShapefileReader::BoundingBox::print() const {
  puts("BoundingBox:");
  printf("\tXmin: %f\n", Xmin);
  printf("\tYmin: %f\n", Ymin);
  printf("\tXmax: %f\n", Xmax);
  printf("\tYmax: %f\n", Ymax);
  printf("\tZmin: %f\n", Zmin);
  printf("\tZmax: %f\n", Zmax);
  printf("\tMmin: %f\n", Mmin);
  printf("\tMmax: %f\n", Mmax);
}

