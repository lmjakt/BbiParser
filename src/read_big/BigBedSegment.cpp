#include "BigBedSegment.h"
#include "DataSegment.h"
#include "BigHeader.h"
#include "R_TreeNode.h"
#include "../util/ByteReader.h"
#include "../util/StringUtil.h"
#include <iostream>
#include <stdlib.h> // for atoi / atof

BigBedSegment::BigBedSegment(std::ifstream& in, R_TreeSubNode* leaf_node,
			     BigHeader* bigHeader)
{
  _fieldCount = bigHeader->fieldCount;
  _definedFieldCount = bigHeader->definedFieldCount;
  _autoSqlString = bigHeader->autoSqlString; // rather dangerous
  _dataCount = bigHeader->dataCount; // count per section ?? or for the whole file?

  //  _chromId = _chromStart = _chromEnd = 0;

  
  // The header gives us the dataCount size, but this is the number for the
  // full set of values. We don't know how many rows in the dataSegment;
  // we can however estimate it by dividing the dataCount / no. of leaf nodes
  // this is not good as it requires all leaf nodes to have been read in (as
  // this is not described anywhere else.
  
  unsigned int data_count_estimate = _dataCount / leaf_node->leaf_node_no();
  if( _dataCount % leaf_node->leaf_node_no() )
    ++data_count_estimate;

  ////// UNHEALTHY CODE  //////
  data_count_estimate = 512;
  //// This is not documented anywhere, but seems to be the most common
  ///  number of entries per block

  // DataSegment is just a simple struct, whose ->data member
  // needs to be deleted later.
  // readDataSegment handless decompression.
  DataSegment data = readDataSegment(in, leaf_node, bigHeader);

  if(data.error_code){
    std::cerr << "BigBedSegment unable to read in data" << std::endl;
    return; // if there is an error data.data should be 0
  }
  
  _chromId.reserve(data_count_estimate);
  _chromStart.reserve(data_count_estimate);
  _chromEnd.reserve(data_count_estimate);

  // _chromId = new unsigned int[_dataCount];  
  // _chromStart = new unsigned int[_dataCount];
  // _chromEnd = new unsigned int[_dataCount];
  
  _texts.reserve(data_count_estimate);
  ///////// DON'T FORGET TO DELETE THE DATASEGMENT
  ByteReader br(data.data, data.length, leaf_node->parent->bytes_reversed());
  //unsigned int i = 0;
  bool terminated;
  while(br.readState() == ByteReader::GOOD){
    _chromId.push_back(br.ui());
    _chromStart.push_back(br.ui());
    _chromEnd.push_back(br.ui());
    _texts.push_back(br.c_str(terminated));
    if(!terminated)
      std::cerr << "Unterminated optional string: " << _texts.back() << std::endl;
    if(_definedFieldCount > 12){
      std::cerr << "BigBedSegment::_definedFieldCount > 12: don't know what to do" << std::endl;
      continue;
    }
    std::vector<std::string> words = splitString(_texts.back(), '\t');
    if(_definedFieldCount > (words.size() + 3)){
      std::cerr << "words.size and _definedFieldCount do not match: "
		<< _definedFieldCount << " != " << words.size() << " + 3" << std::endl;
    }
    // Ugly Code Warning //
    for(unsigned int i=3; i < _definedFieldCount; ++i){
      std::string word;
      if((i-3) < words.size()) word = words[i-3];
      switch(i){
      case 3:
	_names.push_back( word );
	break;
      case 4:
	_scores.push_back( atof(word.c_str()) );
	break;
      case 5:
	_strands.push_back( word[0] );
	break;
      case 6:
	_thickStart.push_back( atoi(word.c_str()) );
	break;
      case 7:
	_thickEnd.push_back( atoi(word.c_str()) );
	break;
      case 8:
	_itemRgb.push_back( word );
	break;
      case 9:
	_blockCounts.push_back( atoi(word.c_str()) );
	break;
      case 10:
	_blockSizes.push_back( word );
	break;
      case 11:
	_blockStarts.push_back( word );
	break;
      default:
	std::cerr << "Impossible I say" << std::endl;
      }
    }
  }
  _dataNo = _chromId.size();
  if(_dataNo != data_count_estimate){
    std::cerr << "BigBedSegment data_count_estimate != dataNo: "
	      << data_count_estimate << " != " << _dataNo << std::endl;
  }
}

BigBedSegment::~BigBedSegment()
{
}

unsigned short BigBedSegment::fieldCount()
{
  return(_fieldCount);
}

unsigned short BigBedSegment::definedFieldCount()
{
  return(_definedFieldCount);
}

std::string BigBedSegment::autoSqlString()
{
  return(_autoSqlString);
}

unsigned int BigBedSegment::dataCount()
{
  return(_dataCount);
}

unsigned int BigBedSegment::dataNo()
{
  return(_dataNo);
}

unsigned int BigBedSegment::count()
{
  return(_dataNo);
}

unsigned int BigBedSegment::chrId()
{
  if(!_dataNo)
    return(0);
  return(_chromId[0]);
}

unsigned int BigBedSegment::last_chrId()
{
  if(!_dataNo)
    return(0);
  return(_chromId[_dataNo-1]);
}

std::vector<unsigned int> BigBedSegment::chromIds()
{
  return(_chromId);
}

std::vector<unsigned int> BigBedSegment::starts()
{
  return(_chromStart);
}

std::vector<unsigned int> BigBedSegment::ends()
{
  return(_chromEnd);
}

float* BigBedSegment::values()
{
  if(_scores.size() != _dataNo || !_scores.size())
    return(0);
  return(_scores.data());
}

std::vector<std::string> BigBedSegment::opt_fields()
{
  return(_texts);
}
