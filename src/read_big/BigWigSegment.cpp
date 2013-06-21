#include "BigWigSegment.h"
#include "BigHeader.h"
#include "R_TreeNode.h"
#include "DataSegment.h"
#include "../util/ByteReader.h"
#include <iostream>
#include <fstream>
#include <zlib.h>


WigRow::WigRow()
  : start(0), end(0), value(0)
{
  // this space left intentionally blank
}

WigRow::WigRow(unsigned int start, unsigned int end, float value)
  : start(start), end(end), value(value)
{
  // this also blank
}


// This constructor takes a leaf node as a starting point. One could also
// imagine making it from higher level nodes, by including the daughter nodes
// but I'm not so sure that this is a good idea, as nodes often contain 256
// sub nodes. (3 levels, of nodes, 5, 256, 256 seemingly the normal split)
// that would seem to merge things rather too far.
BigWigSegment::BigWigSegment(std::ifstream& in, R_TreeSubNode* leaf_node, BigHeader* bigHeader)
{
  error_code = 0;
  _values = 0;
  _starts = 0;
  _ends = 0;
  // seek to the indicated position
  // read disk -> buffer
  // uncompress --> buffer
  DataSegment dataSegment = readDataSegment(in, leaf_node, bigHeader);
  error_code = dataSegment.error_code;
  char* buffer = dataSegment.data;
  unsigned long bufferLength = dataSegment.length;

  if(error_code)
    return;
  
  ByteReader br((char*)buffer, bufferLength, leaf_node->parent->bytes_reversed());
  chromId = br.ui(0);
  chromStart = br.ui(4);
  chromEnd = br.ui(8);
  itemStep = br.ui(12);
  itemSpan = br.ui(16);
  char type_char = buffer[20];
  switch(type_char){
  case 1:
    type=BED;
    break;
  case 2:
    type=VAR;
    break;
  case 3:
    type=FIX;
    break;
  default:
    type=UNDEF;
  }
  reserved = buffer[21];
  itemCount = br.us(22);

  if(type == UNDEF){
    std::cerr << "BigWigSegment Unknown type : " << type_char << std::endl;
    delete []buffer;
    error_code = 5;
    return;
  }
  read_data(br);
  delete []buffer;
}

BigWigSegment::~BigWigSegment()
{
  delete []_values;
  delete []_starts;
  delete []_ends;
}

void BigWigSegment::print_summary()
{
  std::cout << "\n Wig Segment Summary\n"
	    << "Wig Type    : " << type << "\n"
	    << "Chrom id    : " << chromId << "\n"
	    << "chromStart  : " << chromStart << "\n"
	    << "chromEnd    : " << chromEnd << "\n"
	    << "itemStep    : " << itemStep << "\n"
	    << "itemSpan    : " << itemSpan << "\n"
	    << "reserved    : " << (int)reserved << "\n"
            << "itemCount   : " << itemCount << "\n"
	    << std::endl;
}

void BigWigSegment::print_data()
{
  if(!_values){
    std::cerr << "BigWigSegment : No data in values" << std::endl;
    return;
  }
  for(unsigned short i=0; i < itemCount; ++i){
    switch(type){
    case BED:
      std::cout << _starts[i] << "\t" << _ends[i] << "\t" << _values[i] << "\n";
      break;
    case VAR:
      std::cout << _starts[i] << "\t" << _values[i] << "\n";
      break;
    default: // assume fixed
      std::cout << chromStart + i * itemStep << "\t" << _values[i] << "\n";
    }
  }
  std::cout << std::endl;
}

unsigned int BigWigSegment::chrId()
{
  return(chromId);
}

unsigned int BigWigSegment::chrStart()
{
  return(chromStart);
}

unsigned int BigWigSegment::chrEnd()
{
  return(chromEnd);
}

unsigned int BigWigSegment::count()
{
  return(itemCount);
}

const float* BigWigSegment::values()
{
  return( (const float*)_values );
}

std::vector<unsigned int> BigWigSegment::starts()
{
  std::vector<unsigned int> s(itemCount);
  for(unsigned int i=0; i < itemCount; ++i)
    s[i] = beg_pos(i);
  return(s);
}

std::vector<unsigned int> BigWigSegment::ends()
{
  std::vector<unsigned int> e(itemCount);
  for(unsigned int i=0; i < itemCount; ++i)
    e[i] = end_pos(i);
  return(e);
}

std::vector<WigRow> BigWigSegment::rows(int beg, int end)
{
  std::vector<WigRow> r;
  r.reserve(itemCount);
  beg = beg < 0 ? chromStart : beg;
  end = end < 0 ? chromEnd : end;
  
  if(end < (int)chromStart || beg > (int)chromEnd || end < beg)
    return(r);
  for(unsigned int i=0; i < itemCount; ++i){
    if((int)beg_pos(i) <= end &&  (int)end_pos(i) >= beg)
      r.push_back(WigRow(beg_pos(i), end_pos(i), _values[i]));
  }
  return(r);
}

void BigWigSegment::read_data(ByteReader& br)
{
  _values = new float[itemCount];
  if(type == BED || type == VAR)
    _starts = new unsigned int[itemCount];
  if(type == BED)
    _ends = new unsigned int [itemCount];
  
  // without checking anything whatsoever. 
  unsigned short header_size = 24;
  
  // doing the loops within the switch statements is
  // probably faster, but I can't quite bring myself
  // to do this. 
  // similarly changing ByteReader to have stream semantics
  // might also speed things up. (But then we're probably limited
  // by the in.read above, so can't do much about that.
  for(unsigned short i=0; i < itemCount; ++i){
    switch(type){
    case BED:
      _starts[i] = br.ui(header_size + (i * 12));
      _ends[i] = br.ui(4 + header_size + (i * 12));
      _values[i] = br.f(8 + header_size + (i * 12));
      break;
    case VAR:
      _starts[i] = br.ui(header_size + (i * 8));
      _values[i] = br.f(4 + header_size + (i * 8));
      break;
    default:  // assume FIX
      _values[i] = br.f(header_size + (i * 4));
    }
  }
  
}

unsigned int BigWigSegment::beg_pos(unsigned int i)
{
  if(type == FIX)
    return(chromStart + i * itemStep);
  return(_starts[i]);
}

unsigned int BigWigSegment::end_pos(unsigned int i)
{
  switch(type){
  case BED:
    return(_ends[i]);
  case VAR:
    return(_starts[i] + itemSpan);
  default:
    return(chromStart + itemSpan + (i * itemStep));
  }
}
