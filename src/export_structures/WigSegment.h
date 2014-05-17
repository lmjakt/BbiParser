#ifndef WIGSEGMENT_H
#define WIGSEGMENT_H

#include <vector>
#include <string>

#include "../read_big/BigWigSegment.h"
#include "../read_big/BigBedSegment.h"


struct WigRows {
  std::vector<unsigned int> starts;
  std::vector<unsigned int> ends;
  std::vector<float> values;
  WigRows(std::vector<unsigned int>& s, std::vector<unsigned int>& e, std::vector<float>& v);
};

/// TODO INCOMPLETE STUFF BAD DESIGN WARNING ///
// at the moment this class takes either Bed or Wig segments. However, there is no guarantee that Bed segments
// are all derived from the same chromosome, and normally they will include a chromosome descriptor.
// This makes this a particularly bad abstraction. For the time being we can probably leave this though and
// consider a different structure in the future. (Graph based? Chrom -> bed -> pos and bed->chrom->pos ? 

// but lets see if it compiles and works first // 

// This class assumes that the segments given to the constructor are ordered.
// use with care.
class WigSegment {
  enum SegmentType { WIG, BED };
 public:
  const SegmentType segmentType;
  WigSegment(std::vector<BigWigSegment*> segments, unsigned int chromId, std::string chromName);
  WigSegment(std::vector<BigBedSegment*> segments, unsigned int chromId, std::string chromName);
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

  std::vector<float> values();  // only for wig type
  std::vector<std::string> optFields(); // only for bed type

  std::vector<WigRow> rows(int beg=-1, int end=-1);
  WigRows wigRows(int beg=-1, int end=-1);

  std::vector<float> kernel_values(std::vector<int> pos, std::vector<float> kernel);


 private:
  unsigned int _totalSize;
  unsigned int _chrId;
  std::string _chrName;
  std::vector<bool> _segmentOk;
  std::vector<unsigned int> _starts;
  std::vector<unsigned int> _ends;
  std::vector<float> _values;
  std::vector<std::string> _optFields;

  template <class T> void initialise(std::vector<T*>& segments);  // this sets _segmentOk and _totalSize
};

#endif
