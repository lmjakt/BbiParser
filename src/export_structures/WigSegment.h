#ifndef WIGSEGMENT_H
#define WIGSEGMENT_H

#include <vector>
#include <string>

#include "../read_big/BigWigSegment.h"


struct WigRows {
  std::vector<unsigned int> starts;
  std::vector<unsigned int> ends;
  std::vector<float> values;
  WigRows(std::vector<unsigned int>& s, std::vector<unsigned int>& e, std::vector<float>& v);
};

// This class assumes that the segments given to the constructor are ordered.
// use with care.
class WigSegment {
 public:
  WigSegment(std::vector<BigWigSegment*> segments, unsigned int chromId, std::string chromName);
  ~WigSegment();

  unsigned int size();
  unsigned int begin();
  unsigned int end();

  unsigned int elementStart(unsigned int i);
  unsigned int elementEnd(unsigned int i);
  float elementValue(unsigned int i);

  WigRow row(unsigned int i);

  std::vector<unsigned int> starts();
  std::vector<unsigned int> ends();
  std::vector<float> values();

  std::vector<WigRow> rows(int beg=-1, int end=-1);
  WigRows wigRows(int beg=-1, int end=-1);

  std::vector<float> kernel_values(std::vector<int> pos, std::vector<float> kernel);

 private:
  unsigned int totalSize;
  std::vector<bool> segmentOk;
  unsigned int chrId;
  std::string chrName;
  std::vector<unsigned int> _starts;
  std::vector<unsigned int> _ends;
  std::vector<float> _values;
};

#endif
