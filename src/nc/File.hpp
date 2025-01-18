namespace nc {

class File {
public:
  File(const fspath& path, bool analyze = false);
  ~File();

  static void setPrintLimit(u32 n);

private:
  static u32 printLimit;

  int ncid;
  int ndims, nvars, natts, unlimdimid;

private:
  static bool checkPrintLimit(const int& i, const int& end);

  void readDimensions() const;
  void readVariables(int dimid, u8 tabs) const;
  void readAttributes(int varid, int var_natts, u8 tabs) const;
};
} // nc
