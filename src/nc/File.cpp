#include "File.hpp"

#include <cstdio>

#include "netcdf.h"

using namespace nc;

#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); exit(1);}

static int retval;

u32 File::printLimit = 20;

bool File::checkPrintLimit(const int& i, const int& end) {
  bool r = i >= printLimit;
  if (r) printf("...(%d more)\n", end - i);
  return r;
}

File::File(const fspath& path, bool analyze) {
  // Open
  if ((retval = nc_open(path.string().c_str(), NC_NOWRITE, &ncid))) ERR(retval);

  // Get properties
  if ((retval = nc_inq(ncid, &ndims, &nvars, &natts, &unlimdimid))) ERR(retval);

  if (analyze) {
    printf("======================== %ls ========================\n\n", path.filename().c_str());

    printf("ndims: %d, nvars: %d, natts: %d, unlimdimid: %d\n\n", ndims, nvars, natts, unlimdimid);
    readDimensions();

    printf("\n======================== %ls ========================\n", path.filename().c_str());
  }
}

File::~File() {
  if ((retval = nc_close(ncid))) ERR(retval);
}

void File::setPrintLimit(u32 n) { printLimit = n; }

void File::readDimensions() const {
  for (int i = 0; i < ndims; i++) {
    if (checkPrintLimit(i, ndims)) break;

    char name[256];
    size_t length;

    if ((retval = nc_inq_dim(ncid, i, name, &length))) ERR(retval);

    printf("Dimension %d {name: %s, length: %llu}\n\n", i, name, length);
    readVariables(i, 1);
  }
}

void File::readVariables(int dimid, u8 tabs) const {
  for (int i = 0; i < nvars; i++) {
    printTabs(tabs);
    if (checkPrintLimit(i, nvars)) break;

    char name[256];
    nc_type type;
    int dimids[256];
    int var_ndims;
    int var_natts;

    if ((retval = nc_inq_var(ncid, i, name, &type, &var_ndims, dimids, &var_natts))) ERR(retval);

    printf("Variable %d {name: %s, type: %d, ndims: %d, natts: %d}\n", i, name, type, var_ndims, var_natts);

    if (var_natts > 0) {
      printf("\n");
      readAttributes(i, var_natts, tabs + 1);
      printf("\n");
    } else {
      printf("\n");
    }
  }
}

void File::readAttributes(int varid, int var_natts, u8 tabs) const {
  for (int i = 0; i < var_natts; i++) {
    printTabs(tabs);
    if (checkPrintLimit(i, var_natts)) break;

    char name[256];
    nc_type type;
    size_t length;

    if ((retval = nc_inq_attname(ncid, varid, i, name))) ERR(retval);
    if ((retval = nc_inq_att(ncid, varid, name, &type, &length))) ERR(retval);

    printf("Attribute %d {name: %s, type: %d, length: %llu}\n", i, name, type, length);
  }
}
