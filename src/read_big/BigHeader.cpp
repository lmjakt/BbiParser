#include <fstream>
#include "BigHeader.h"
#include "../util/ByteReader.h"
#include <iostream>

const unsigned int header_size = BIG_HEADER_SIZE;

BigHeader::BigHeader(std::ifstream& in)
{
  error = 0;
  in.seekg(0, std::ios_base::end);
  if( in.tellg() <= header_size ){
    std::cerr << "BigHeader::Bigheader file smaller than " << header_size << " bytes" << std::endl;
    error = 1;
    return;
  }
  in.seekg(0, std::ios_base::beg);
  char* bytes = new char[header_size];
  in.read(bytes, header_size);
  if(in.eof() || in.fail() || in.bad()){
    std::cerr << "BigHeader::BigHeader failed to read bytes, obtained stream error: " 
	      << in.eof() << ", " << in.fail() << "," << in.bad() << std::endl;
    error = 2;
    delete []bytes;
    return;
  }
  init((const char*)bytes);
  readAdditional(in);
  delete []bytes;
}

void BigHeader::init(const char* bytes)
{
  error = 0;
  // We don't need to care about our own endian-ness,
  // due to the magic of the hexadecimal specification.
  reverse_bytes = false; // fairly likely
  unsigned int magic = *(unsigned int*)bytes;
  if((magic != BIGWIG_MAGIC) && (magic != BIGBED_MAGIC)){
    magic = __builtin_bswap32(magic);
    reverse_bytes = true;
    if((magic != BIGWIG_MAGIC) && (magic != BIGBED_MAGIC)){
      std::cerr << "BigHeader::BigHeader unable to obtain appropriate magic " << magic << std::endl;
      error = 4;
      return;
    }
  }
  isBigBed = (magic == BIGBED_MAGIC);
  
  ByteReader br(bytes, header_size, reverse_bytes);
  
  version = br.us(4);
  zoomLevels = br.us(6);
  chromosomeTreeOffset = br.ul(8);
  fullDataOffset = br.ul(16);
  fullIndexOffset = br.ul(24);
  fieldCount = br.us(32);
  definedFieldCount = br.us(34);
  autoSqlOffset = br.ul(36);
  totalSummaryOffset = br.ul(44);
  uncompressBufSize = br.ui(52);
  reserved = br.ul(56);
  
  // at this point check if anything seems a little bit strange. At the time of
  // writing the current version was 4, so we don't expect more than 20.
  // note that the documentation I found is for version 3, so we may already be a little
  // bit late.
  if(version > 20)
    std::cerr << "BigHeader::BigHeader Unexpected version number: " << version << std::endl;
  // a very large number of zoomlevels also seems unexpected
  if(zoomLevels > 30)
    std::cerr << "BigHeader::BigHeader Unexpected number of zoom levels: " << zoomLevels << std::endl;
  // 100 MBytes seems maybe a bit much for the uncompress buffer size?
  if(uncompressBufSize > 1e8)
    std::cerr << "BigHeader::BigHeader Unexpectedly large uncompress buffer required?" << std::endl;

  // finally reserved is reserved for future use. If reserved isn't 0, then it suggests version is rather
  // larger than expected.
  if(reserved)
    std::cerr << "Reserve is non-zero, there may be a need to update the parser" << std::endl;
}

void BigHeader::readAdditional(std::ifstream& in)
{
  dataCount = 0;
  // dataCount is supposed to be found at the beginning of the dataSection
  in.seekg(fullDataOffset);
  if(!in.good()){
    std::cerr << "BigHeader unable to seek to position of fullDataOffset: " << fullDataOffset << std::endl;
  }else{
    in.read((char*)&dataCount, 4);
  }
  
  if(!in.good())
    std::cerr << "BigHeader unable to read data to dataCount" << std::endl;
  
  // to get the sql header we can make use of C++ magic
  if(autoSqlOffset){
    in.seekg(autoSqlOffset);
    getline(in, autoSqlString, '\0');
    //    in >> autoSqlString;
    if(!in.good())
      std::cerr << "BigHeader unable to read in the autoSqlString" << std::endl;
  }
}

BigHeader::~BigHeader()
{
  // do something one day when necessary.
}

void BigHeader::print_summary()
{
  std::cout << "printing summary from : " << this << std::endl;
  std::cout << "\treverse_bytes  \t" << reverse_bytes << "\n"
	    << "\tisBigBed       \t" << isBigBed << "\n"
	    << "\tversion        \t" << version << "\n"
	    << "\tzoomLevels     \t" << zoomLevels << "\n"
	    << "\tchrom tree off \t" << chromosomeTreeOffset << "\n"
	    << "\tdata offset    \t" << chromosomeTreeOffset << "\n"
	    << "\tindex offset   \t" << fullIndexOffset << "\n"
	    << "\tfield count    \t" << fieldCount << "\n"
	    << "\tdef field count\t" << definedFieldCount << "\n"
	    << "\tautoSqlOffset  \t" << autoSqlOffset << "\n"
	    << "\tsummary offset \t" << totalSummaryOffset << "\n"
	    << "\tuncompress size\t" << uncompressBufSize << "\n" 
	    << "\tdataCount      \t" << dataCount << "\n"
	    << "autoSqlString: \n" << autoSqlString
	    << std::endl;
  
}
