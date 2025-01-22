#include "File.hpp"

#include <cstdio>

#include "netcdf.h"

using namespace nc;

int File::status;
u32 File::printLimit = 20;

const char* const types[]{
  "NC_NAT",    "NC_BYTE",  "NC_CHAR",   "NC_SHORT", "NC_INT (NC_LONG)", "NC_FLOAT",
  "NC_DOUBLE", "NC_UBYTE", "NC_USHORT", "NC_UINT",  "NC_INT64",         "NC_UINT64",
  "NC_STRING", "NC_VLEN",  "NC_OPAQUE", "NC_ENUM",  "NC_COMPOUND",
};

void File::error(const int& e) {
  printf("\033[1;31;49mError: %s\033[0m\n", nc_strerror(e));
  exit(EXIT_FAILURE);
}

bool File::checkPrintLimit(const int& i, const int& end) {
  bool r = i >= (int)printLimit;
  if (r) printf("...(%d more)\n", end - i);
  return r;
}

File::File(const fspath& path, bool print) {
  // Open
  if ((status = nc_open(path.string().c_str(), NC_NOWRITE, &ncid))) error(status);

  // Get properties
  if ((status = nc_inq(ncid, &ndims, &nvars, &natts, &unlimdimid))) error(status);

  if (print) {
    printf("======================== %ls ========================\n\n", path.filename().c_str());

    printf("ndims: %d, nvars: %d, natts: %d, unlimdimid: %d\n\n", ndims, nvars, natts, unlimdimid);
    readDimensions();

    printf("\n======================== %ls ========================\n", path.filename().c_str());
  }
}

File::~File() {
  if ((status = nc_close(ncid))) error(status);
}

void File::setPrintLimit(u32 n) { printLimit = n; }

u32 File::getDimensionSize(int dim) const {
  size_t len;
  if ((status = nc_inq_dimlen(ncid, dim, &len))) error(status);
  return static_cast<u32>(len);
}

void File::readDimensions() const {
  for (int i = 0; i < ndims; i++) {
    if (checkPrintLimit(i, ndims)) break;

    char name[NC_MAX_NAME];
    size_t length;

    if ((status = nc_inq_dim(ncid, i, name, &length))) error(status);

    printf("Dimension %d {name: %s, length: %llu}\n\n", i, name, length);
    readVariables(i, 1);
  }
}

void File::readVariables(int dimid, u8 tabs) const {
  for (int i = 0; i < nvars; i++) {
    printTabs(tabs);
    if (checkPrintLimit(i, nvars)) break;

    char name[NC_MAX_NAME];
    nc_type type;
    int dimids[NC_MAX_VAR_DIMS];
    int var_ndims;
    int var_natts;

    if ((status = nc_inq_var(ncid, i, name, &type, &var_ndims, dimids, &var_natts))) error(status);

    printf("Variable %d {name: %s, type: %s, ndims: %d, natts: %d}\n", i, name, types[type], var_ndims, var_natts);

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

    char name[NC_MAX_NAME];
    nc_type type;
    size_t length;

    if ((status = nc_inq_attname(ncid, varid, i, name))) error(status);
    if ((status = nc_inq_att(ncid, varid, name, &type, &length))) error(status);

    printf("Attribute %d {name: %s, type: %s, length: %llu}\n", i, name, types[type], length);
  }
}
