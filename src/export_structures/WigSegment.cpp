#include "WigSegment.h"
#include "../read_big/BigWigSegment.h"
#include <stdlib.h>

WigRows::WigRows(std::vector<unsigned int>& s, std::vector<unsigned int>& e, std::vector<float>& v)
  : starts(s), ends(e), values(v)
{

}

WigSegment::WigSegment(std::vector<BigWigSegment*> segments, unsigned int chromId, std::string chromName)
  : segmentType(WIG), _chrId(chromId), _chrName(chromName)
{
  
  initialise(segments);

  /*
  // total size includes only those segments which have the specified chromId
  // this shouldn't be necessary to check, but..
  _segmentOk.resize(segments.size());
  _totalSize = 0;
  for(unsigned int i=0; i < segments.size(); ++i){
    _segmentOk[i] = false;
    if(segments[i]->chrId() == _chrId){
      _segmentOk[i] = true;
      _totalSize += segments[i]->count();
    }
  }
  _starts.reserve(_totalSize);
  _ends.reserve(_totalSize);
  _values.reserve(_totalSize);

  for(unsigned int i=0; i < segments.size(); ++i){
    std::vector<unsigned int> s = segments[i]->starts(); // incredibly ugly makes me think of using
    std::vector<unsigned int> e = segments[i]->ends();   // pointers instead.
    _starts.insert(_starts.end(), s.begin(), s.end());
    _ends.insert(_ends.end(), e.begin(), e.end());
    _values.insert( _values.end(), segments[i]->values(), segments[i]->values() + segments[i]->count() );
  }
  */
}

WigSegment::WigSegment(std::vector<BigBedSegment*> segments, unsigned int chromId, std::string chromName)
  : segmentType(BED), _chrId(chromId), _chrName(chromName)
{
  initialise(segments);
}

WigSegment::~WigSegment()
{
  // the segments belong elsewhere
}

unsigned int WigSegment::size()
{
  return(_totalSize);
}

unsigned int WigSegment::begin()
{
  if(!_starts.size())
    return(0);
  return(_starts[0]);
}

unsigned int WigSegment::end()
{
  if(!_ends.size())
    return(0);
  return( _ends.back() );
}

unsigned int WigSegment::elementStart(unsigned int i)
{
  if(i < _starts.size())
    return(_starts[i]);
  return(0);
}

unsigned int WigSegment::elementEnd(unsigned int i)
{
  if(i < _ends.size())
    return(_ends.size());
  return(0);
}

float WigSegment::elementValue(unsigned int i)
{
  if(i < _values.size())
    return(_values[i]);
  return(0);
}

WigRow WigSegment::row(unsigned int i)
{
  if(i < _values.size())
    return(WigRow(_starts[i], _ends[i], _values[i]));

  WigRow wr;
  return(wr);
}

std::vector<unsigned int> WigSegment::starts()
{
  return(_starts);
}

std::vector<unsigned int> WigSegment::ends()
{
  return(_ends);
}

std::vector<float> WigSegment::values()
{
  return(_values);
}

std::vector<std::string> WigSegment::optFields()
{
  return(_optFields);
}

std::vector<WigRow> WigSegment::rows(int beg, int end)
{
  std::vector<WigRow> wr;
  beg = beg < 0 ? (_starts.front()) : beg;
  end = end < 0 ? (_ends.back()) : end;
  if(beg < end)
    return(wr);
  if(beg > (int)(_ends.back()) || end < (int)(_starts.front()))
    return(wr);
  
  wr.reserve(_totalSize);
  for(unsigned int i=0; i < _starts.size(); ++i){
    if(end >= (int)_starts[i] && beg <= (int)_ends[i])
      wr.push_back(WigRow(_starts[i], _ends[i], _values[i]));
  }
  return(wr);
}

WigRows WigSegment::wigRows(int beg, int end)
{
  std::vector<unsigned int> s;
  std::vector<unsigned int> e;
  std::vector<float> v;
  s.reserve(_totalSize);
  e.reserve(_totalSize);
  v.reserve(_totalSize);
  for(unsigned int i=0; i < _totalSize; ++i){
    if(end >= (int)_starts[i] && beg <= (int)_ends[i]){
      s.push_back(_starts[i]);
      e.push_back(_ends[i]);
      v.push_back(_values[i]);
    }
  }
  return(WigRows(s, e, v));
}

// use the kernel to calculate smoothened signals
// the kernel steps should be in single base pairs
// and the sum of the kernel should be 1.0. Hmm.
// the kernel is one-sided with index indicating distance from center
std::vector<float> WigSegment::kernel_values(std::vector<int> pos, std::vector<float> kernel)
{
  // we don't actually care too much about the details. We will assume that pos is ordered;
  // if this isn't the case the result is undefined
  
  std::vector<float> v(pos.size());
  int kernel_size = (int)kernel.size();

  unsigned int m_i = 0;
  for(unsigned int i=0; i < v.size(); ++i){
    v[i] = 0;
    while( (pos[i] - (int)_ends[m_i]) >= kernel_size && m_i < _starts.size()){
      ++m_i;
    }
    unsigned int mm_i = m_i;
    while( (abs(_ends[mm_i] - pos[i]) < kernel_size 
	    ||
	    abs(_starts[mm_i] - pos[i]) < kernel_size)
	   && mm_i < _starts.size()){
      unsigned int p = _starts[mm_i];
      while(p <= _ends[mm_i] && abs( p - pos[i] ) < kernel_size){
	v[i] += _values[mm_i] * kernel[abs(p - pos[i]) ];
	++p;
      }
      ++mm_i;
    }
  }
  return( v );
}

template<class T> void WigSegment::initialise(std::vector<T*>& segments)
{
  _totalSize = 0;
  _segmentOk.resize(segments.size());
  for(unsigned int i=0; i < segments.size(); ++i){
    _segmentOk[i] = false;
    if(segments[i]->chrId() == _chrId){
      _segmentOk[i] = true;
      _totalSize += segments[i]->count();
    }
  }
  _starts.reserve(_totalSize);
  _ends.reserve(_totalSize);
  _values.reserve(_totalSize);
  if(segmentType == BED){  // no values
    _values.insert(_values.begin(), _totalSize, 1.0);
    _optFields.reserve(_totalSize);
  }else{
    _values.reserve(_totalSize);
  }

  for(unsigned int i=0; i < segments.size(); ++i){
    if(!_segmentOk[i])
      continue;
    std::vector<unsigned int> s = segments[i]->starts();
    std::vector<unsigned int> e = segments[i]->ends();
    _starts.insert(_starts.end(), s.begin(), s.end());
    _ends.insert(_ends.end(), e.begin(), e.end());
    if(segmentType == WIG)
      _values.insert(_values.end(), segments[i]->values(), segments[i]->values() + segments[i]->count() );
    if(segmentType == BED){
      std::vector<std::string> of = segments[i]->opt_fields();
      _optFields.insert(_optFields.end(), of.begin(), of.end());
    }
  }
  
}
