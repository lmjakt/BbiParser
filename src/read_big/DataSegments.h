#ifndef DATASEGMENTS_H
#define DATASEGMENTS_H

// simply some structs for keeping data in.

// a minimal entry
// the actual data file also contains the chrom index
// but there doesn't seem to be much point in this as
// segments are organised by location.

struct R_TreeSubNode;
struct BigHeader;

struct MinBigBedEntry {
  unsigned int start;
  unsigned int end;
};

struct MaxBigBedEntry {
  unsigned int start;
  unsigned int end;
  char* additional;
};

class BigWigSegment {
 public:
  BigWigSegment(ifstream& in, R_TreeSubNode* node, BigHeader* bigHeader);
  ~BigWigSegment();
  
 private:
  enum WigType { BED=1, VAR=2, FIX=3 };
  unsigned int chromId;
  unsigned int chromStart;
  unsigned int chromEnd;
  unsigned int itemStep; // only used in FIX type
  unsigned int itemSpan;
  WigType type;
  char reserved;
  unsigned short itemCount;

  float* values; // used for all types.
  unsigned int* starts; // used with BED and VAR
  unsigned int* ends;   // only used with BED
};

#endif
