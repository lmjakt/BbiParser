#ifndef BIGBEDSEGMENT_H
#define BIGBEDSEGMENT_H

#include <fstream>
#include <string>
#include <vector>

class R_TreeSubNode;
class BigHeader;

class BigBedSegment {
 public:
  BigBedSegment(std::ifstream& in, R_TreeSubNode* leaf_node,
		BigHeader* bigHeader);
  ~BigBedSegment();

  unsigned short fieldCount();
  unsigned short definedFieldCount();
  std::string autoSqlString();
  unsigned int dataCount();
  unsigned int dataNo();
  unsigned int count(); // same as dataNo() for compatibility with BigWig
  unsigned int chrId(); // returns the first chromId
  unsigned int last_chrId();
  
  std::vector<unsigned int> chromIds();
  std::vector<unsigned int> starts();
  std::vector<unsigned int> ends();
  float* values(); // always returns 0
  std::vector<std::string> opt_fields();

  

 private:
  // the following can all be found in the BigHeader
  // rather than anywhere in the nodes. However, it's useful
  // to package these up in the BigBedSegment structure
  unsigned short _fieldCount;
  unsigned short _definedFieldCount;
  std::string _autoSqlString;
  unsigned int _dataCount; // this is set in the bigHeader
  unsigned int _dataNo;  // this is the actual number found
  
  // The Bed format is extensible, with only 3 values being
  // mandatory (chromId, chromStart, chromEnd).
  //
  // only the three mandatory variables are stored in a binary format
  // the rest is just in a string, which we don't know how to handle.
  // For now, we'll leave it as is

  std::vector<unsigned int> _chromId;
  std::vector<unsigned int> _chromStart;
  std::vector<unsigned int> _chromEnd;

  // The folllowing are all optional, but defined fields, they must
  // follow in the specified order
  // to determine if they're defined, check the value of _definedFieldCount
  // if more than 3, than they'll be defined in the following order

  std::vector<std::string> _names;
  std::vector<float> _scores; // supposed to be a value between 0-1000
  std::vector<char> _strands;
  std::vector<int> _thickStart; // ??
  std::vector<int> _thickEnd;
  std::vector<std::string> _itemRgb; // R,G,B --> numeric ?
  std::vector<int> _blockCounts;
  std::vector<std::string> _blockSizes; // these should be vector<vector>, but...
  std::vector<std::string> _blockStarts; 

  std::vector<std::string> _texts;
  
  //char* copyString(std::ifstream in, size_t off_set, size_t length);
  //  std::vector<std::string> splitString(std::string& line, char delim);

};

#endif
