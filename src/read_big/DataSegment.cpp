#include "DataSegment.h"
#include "BigHeader.h"
#include "R_TreeNode.h"
#include <iostream>
#include <zlib.h>

DataSegment readDataSegment(std::ifstream& in, R_TreeSubNode* leaf_node,
			    BigHeader* bigHeader)
{
  DataSegment ds;
  if(!leaf_node->dataSize){
    std::cerr << "BigWigSegment :: tried to initialise from non-leaf node." << std::endl;
    ds.error_code = 1;
    return(ds);
  }
  in.seekg(leaf_node->dataOffset);
  if(!in.good()){
    std::cerr << "BigWigSegment :: unable to seek to : " 
	      << leaf_node->dataOffset << std::endl;
    ds.error_code = 2;
    return(ds);
  }
  char* buffer = new char[ leaf_node->dataSize ];
  in.read(buffer, leaf_node->dataSize);
  if(!in.good()){
    std::cerr << "BigWigSegment :: unable to read in data segment" << std::endl;
    ds.error_code = 3;
    delete []buffer;
    return(ds);
  }
  ds.length = leaf_node->dataSize; // this will be changed by uncompress
  // if the data is compressed then uncompressBufsize will be set in the bigheader
  if(bigHeader->uncompressBufSize){
    ds.length = bigHeader->uncompressBufSize;
    char* uncomp_buffer = new char[ds.length];
    int res = uncompress((unsigned char*)uncomp_buffer, &ds.length, 
			 (unsigned char*)buffer, leaf_node->dataSize);
    if(res != Z_OK){
      std::cerr << "BigWigSegment :: unable to uncompress data segment: " 
		<< res << std::endl;
      ds.error_code = 4;
      delete []buffer;
      delete []uncomp_buffer;
      return(ds);
    }
    // at this point let's swap the buffers.
    delete []buffer;
    ds.data = uncomp_buffer;
  }
  return(ds);
}
