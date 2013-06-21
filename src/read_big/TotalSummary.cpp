#include "TotalSummary.h"
#include "../util/ByteReader.h"
#include <iostream>

const unsigned int summary_length = TOTAL_SUMMARY_LENGTH;

TotalSummary::TotalSummary()
{
  error = 1;
  basesCovered = 0;
}

TotalSummary::TotalSummary(std::ifstream& in, bool reverse)
{
  char* bytes = new char[summary_length];
  in.read(bytes, summary_length);
  if(!in.good()){
    delete []bytes;
    error = 2;
    std::cerr << "TotalSummary::TotalSummary Unable to read from stream" << std::endl;
    return;
  }
  init(bytes, reverse);
}

TotalSummary::TotalSummary(const char* bytes, bool reverse)
{
  init(bytes, reverse);
}

void TotalSummary::init(const char* bytes, bool reverse)
{
  reverse_bytes = reverse;
  ByteReader br(bytes, summary_length, reverse_bytes);
 
  basesCovered = br.ul(0);
  minVal = br.d(8);
  maxVal = br.d(16);
  sumData = br.d(24);
  sumSquares = br.d(32);
}

void TotalSummary::print_summary()
{
  std::cout << "\tBases Covered: \t" << basesCovered << "\n"
	    << "\tminVal       : \t" << minVal << "\n"
	    << "\tmaxVal       : \t" << maxVal << "\n"
	    << "\tsumData      : \t" << sumData << "\n"
	    << "\tsumSquares   : \t" << sumSquares << "\n" << std::endl;
}
