#ifndef ZOOMHEADER_H
#define ZOOMHEADER_H

#include <fstream>

class ZoomHeader
{
 public:
  ZoomHeader(); // necessary if we want to keep in a std::vector
  ZoomHeader(std::ifstream& in, bool reverse);
  ZoomHeader(const char* bytes, bool reverse);

  ~ZoomHeader(){}

  // member variables are public;

  unsigned int reductionLevel; // number of bases summarised in level
  unsigned int reserved; // --> 0
  unsigned long dataOffset;
  unsigned int indexOffset;

  bool reverse_bytes;
  int error;

  void print_summary();

 private:
  void init(const char* bytes, bool reverse_bytes);
};

#endif
