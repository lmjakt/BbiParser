#ifndef BIGBEDSEGMENT_H
#define BIGBEDSEGMENT_H

#include <fstream>

class R_TreeSubNode;
class BigHeader;

class BigBedSegment {
 public:
  BigBedSegment(std::ifstream& in, R_TreeSubNode* leaf_node,
		BigHeader* bigHeader);
  ~BigBedSegment();

 private:
  // the following can all be found in the BigHeader
  // rather than anywhere in the nodes. However, it's useful
  // to package these up in the BigBedSegment structure
  unsigned short fieldCount;
  unsigned short definedFieldCount;
  char* autoSqlString;
  unsigned int dataCount;
  
  // The Bed format is extensible, with only 3 values being
  // mandatory (chromId, chromStart, chromEnd).
  //
  // only the three mandatory variables are stored in a binary format
  // the rest is just in a string, which we don't know how to handle.
  // For now, we'll leave it as is
  
  unsigned int* chromId;
  unsigned int* chromStart;
  unsigned int* chromEnd;
  char** text;
  
  char* copyString(std::ifstream in, size_t off_set, size_t length);

};

#endif
