#ifndef BIGWIGSEGMENT_H
#define BIGWIGSEGMENT_H

// simply some structs for keeping data in.

// a minimal entry
// the actual data file also contains the chrom index
// but there doesn't seem to be much point in this as
// segments are organised by location.

#include <fstream>
#include <vector>

struct R_TreeSubNode;
struct BigHeader;
class ByteReader;

struct WigRow {
  unsigned int start;
  unsigned int end;
  float value;
  WigRow();
  WigRow(unsigned int start, unsigned int end, float value);
};


class BigWigSegment {
 public:
  BigWigSegment(std::ifstream& in, R_TreeSubNode* leaf_node, BigHeader* bigHeader);
  ~BigWigSegment();
  
  enum WigType { BED=1, VAR=2, FIX=3, UNDEF };

  void print_summary();
  void print_data();

  unsigned int chrId();
  unsigned int chrStart();
  unsigned int chrEnd();
  unsigned int count();
  
  const float* values();
  std::vector<unsigned int> starts(); // starts and ends need to be
  std::vector <unsigned int> ends();  // copies as they may not exist
  std::vector<std::string> opt_fields(); // returns an empty vector
  std::vector<WigRow> rows(int beg=-1, int end=-1);
  

  WigType wigType();

 private:
  unsigned int chromId;
  unsigned int chromStart;
  unsigned int chromEnd;
  unsigned int itemStep; // only used in FIX type
  unsigned int itemSpan;
  WigType type;
  char reserved;
  unsigned short itemCount;

  float* _values; // used for all types.
  unsigned int* _starts; // used with BED and VAR
  unsigned int* _ends;   // only used with BED

  int error_code;
 
  void read_data(ByteReader& br);
  unsigned int beg_pos(unsigned int i); // NO Error check.
  unsigned int end_pos(unsigned int i); // NO Error check.
};

#endif
