#ifndef DATASEGMENT_H
#define DATASEGMENT_H

#include <fstream>
class R_TreeSubNode;
class BigHeader;

// does not do any desctructor business.
// just used to shuffle data around.
struct DataSegment {
  char* data;
  size_t length;
  int error_code;
  DataSegment(){
    data = 0;
    length = error_code = 0;
  }
};

// if the node is not a leaf_node this function will simply set
// error_code to 1 and return 0.
DataSegment readDataSegment(std::ifstream& in, R_TreeSubNode* leaf_node, 
			    BigHeader* bigHeader);

#endif
