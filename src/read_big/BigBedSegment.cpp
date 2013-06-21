#include "BigBedSegment.h"
#include "DataSegment.h"
#include "BigHeader.h"
#include "../util/ByteReader.h"
#include <iostream>

BigBedSegment::BigBedSegment(std::ifstream& in, R_TreeSubNode* leaf_node,
			     BigHeader& bigHeader)
{
  fieldCount = bigHeader->fieldCount;
  definedFieldCount = bigHeader->definedFieldCount;
  autoSqlString = bigHeader->autoSqlString; // rather dangerous
  dataCount = bigHeader->dataCount; // count per section ?? or for the whole file?

  chromId = chromStart = chromEnd = 0;
  text = 0;

  DataSegment data readDataSegment(in, leaf_node, bigHeader);

  if(data.error_code){
    std::cerr << "BigBedSegment unable to read in data" << std::endl;
    return;
  }
  
  chromId = new unsigned int[dataCount];  // this ought not to be neccessary,, 
  chromStart = new unsigned int[dataCount];
  chromEnd = new unsigned int[dataCount];
  text = new char*[dataCount];

  ///////// DON'T FORGET TO DELETE THE DATASEGMENT

}

