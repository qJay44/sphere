#pragma once

namespace nc {

class File {
public:
  File(const fspath& path, bool print = false);
  ~File();

  static void setPrintLimit(u32 n);

  u32 getDimensionSize(int dim) const;

protected:
  static int status;
  static u32 printLimit;

  int ncid;
  int ndims, nvars, natts, unlimdimid;

  static void error(const int& e);

private:
  static bool checkPrintLimit(const int& i, const int& end);

  void readDimensions() const;
  void readVariables(int dimid, u8 tabs) const;
  void readAttributes(int varid, int var_natts, u8 tabs) const;
};
} // namespace nc
