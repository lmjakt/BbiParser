#include <iostream>
#include "../util/ByteReader.h"
#include "ZoomHeader.h"

const unsigned int zoomHeaderLength = 24;

ZoomHeader::ZoomHeader()
{
  error = 1; // not inited
  reductionLevel = reserved = 0;
  dataOffset = indexOffset = 0;
}

ZoomHeader::ZoomHeader(std::ifstream& in, bool reverse)
{
  char* bytes = new char[zoomHeaderLength];
  in.read(bytes, zoomHeaderLength);
  if(!in.good()){
    std::cerr << "ZoomHeader::ZoomHeader unable to read from stream" << std::endl;
    error = 2;
    delete []bytes;
    return;
  }
  init(bytes, reverse);
  delete []bytes;
}

ZoomHeader::ZoomHeader(const char* bytes, bool reverse)
{
  init(bytes, reverse);
}

void ZoomHeader::init(const char* bytes, bool reverse)
{
  error = 0;
  reverse_bytes = reverse;
  ByteReader br(bytes, zoomHeaderLength, reverse_bytes);
  
  reductionLevel = br.ui(0);
  reserved = br.ui(4);
  dataOffset = br.ul(8);
  indexOffset = br.ul(16);
}

void ZoomHeader::print_summary()
{
  std::cout << "\tReduction level:\t" << reductionLevel << "\n"
	    << "\tReserved       :\t" << reserved << "\n"
	    << "\tdataOffset     :\t" << dataOffset << "\n"
	    << "\tindexOffset    :\t" << indexOffset << "\n" << std::endl;
}
