#ifndef TOTALSUMMARY_H
#define TOTALSUMMARY_H

#include <fstream>

#define TOTAL_SUMMARY_LENGTH 40

class TotalSummary
{
 public:
  TotalSummary();
  TotalSummary(std::ifstream& in, bool reverse);
  TotalSummary(const char* bytes, bool reverse);
  
  ~TotalSummary(){};
  // public member variables.
  unsigned long basesCovered;
  double minVal;
  double maxVal;
  double sumData;
  double sumSquares;

  bool reverse_bytes;
  int error;

  void print_summary();
 private:
  void init(const char* bytes, bool reverse);
};

#endif
