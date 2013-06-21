#ifndef BIGFILE_H
#define BIGFILE_H

// This file should define a number of interfaces for the
// BigWig and BigBed file formats

// which boils down to providing some basic accessor functions
// and perhaps some utilities to standardise data from different
// files (eg. to give evenly spaced data with no holes in it).

#include <map>
#include <string>
#include <vector>
#include <fstream>
#include "../export_structures/WigSegment.h"

class BigHeader;
class ZoomHeader;
class TotalSummary;
class ChromosomeBTree;
class R_Tree;

struct ChromInfo;

class BigFile {
 public:
  BigFile(const char* fileName);
  ~BigFile();

  std::string errorString;
  bool good();
  
  WigSegment* segment(std::string chrName, unsigned int beg, unsigned int end);

 private:
  std::ifstream in;
  BigHeader* header;
  std::vector<ZoomHeader*> zoomHeaders;
  TotalSummary* summary;
  ChromosomeBTree* chromTree;
  R_Tree* rTree;

  // for convenience let's have
  std::map<unsigned int, ChromInfo> chromosomes;
  std::map<std::string, unsigned int> chrom_NameIndex;
  
  std::vector<std::string> warnings;
  
};


#endif
