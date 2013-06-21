#ifndef BIGHEADER_H
#define BIGHEADER_H

/*
  Contains the member variables for a BigWig or BigBed file header.
  Reads these from an ifstream and tries to do a little bit of sanity
  checking on the numbers to check that these are in general correct.

  Make an alternate constructor that takes a const char* pointer for
  as an argument.

  We leave all the relevant values as public rather than have a bunch of
  accessor functions or others. Simplicity over safety.

  Note that this code is unlikely to be as correct / safe as Jim Kent's
  original code. I'm writing it partly to make sure I understand the file
  structure, and partially to allow me to look at the data without needing
  to take too much time in reading documentation.
  
  The code assumes that long = 64 bits, and similar terrible things.

*/

#include <fstream>
#include <string>

#define BIGWIG_MAGIC 0x888FFC26
#define BIGBED_MAGIC 0x8789F2EB
#define BIG_HEADER_SIZE 64

class BigHeader {
 public:
  BigHeader(std::ifstream& in);
  
  ~BigHeader();
  void print_summary();
  // reverse_bytes is determined by comparing the magic.
  // the magic is specified as a hexadecimal number and this
  // does indeed do some magic. On a small endian system it
  // will end up reordering after reading or writing. The
  // end result of this is that if the magic matches, I don't
  // have to reverse bytes as it means that the writing was done
  // on a system of the same endian-ness as my current system.

  int error; // 0 if ok, 

  bool reverse_bytes;
  bool isBigBed;  // if false then is BigWig

  unsigned int magic;
  unsigned short version;
  unsigned short zoomLevels;
  unsigned long chromosomeTreeOffset;
  unsigned long fullDataOffset;
  unsigned long fullIndexOffset;
  unsigned short fieldCount;  // BigBed only, 0 in bigwig
  unsigned short definedFieldCount;
  unsigned long autoSqlOffset;
  unsigned long totalSummaryOffset;
  unsigned int uncompressBufSize;
  unsigned long reserved; // not used

  // these last two are not really part of the header, but are better to define here
  unsigned int dataCount;
  std::string autoSqlString;

 private:
  void init(const char* bytes);
  void readAdditional(std::ifstream& in);

};

#endif
