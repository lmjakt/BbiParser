#include "BigFile.h"
#include "BigHeader.h"
#include "ZoomHeader.h"
#include "TotalSummary.h"
#include "ChromosomeBTree.h"
#include "R_Tree.h"
#include "R_TreeNode.h"
#include "BigWigSegment.h"
#include <vector>
#include <map>
#include <iostream>
#include <fstream>


BigFile::BigFile(const char* fileName)
{
  header = 0;
  summary = 0;
  chromTree = 0;
  rTree = 0;

  in.open(fileName);
  if(!in.good()){
    errorString = std::string("Unable to open file: ") +  fileName;
    return;
  }
  header = new BigHeader(in);
  if(header->error){
    errorString = std::string("Unable to read header from: ") + fileName;
    return;
  }
  in.seekg(BIG_HEADER_SIZE); // defed in BigHeader.h
  if(!in.good()){
    errorString = std::string("Unable to seek to end of header: ") + fileName;
    return;
  }
  // The zoom headers follow immediately after the main header
  for(unsigned int i=0; i < header->zoomLevels; ++i){
    ZoomHeader* zh = new ZoomHeader(in, header->reverse_bytes);
    if(zh->error){
      errorString = "Unable to read all zoomHeaders";
      delete zh;
      return;
    }
    zoomHeaders.push_back(zh);
  }
  // if we the header has defined a totalSummaryOffset, then read this in
  // 
  if(header->totalSummaryOffset);
  in.seekg(header->totalSummaryOffset);
  if(!in.good()){
    errorString = "Unable to seek to beginning of totalSummary";
    return;
  }
  summary = new TotalSummary(in, header->reverse_bytes);

  in.seekg(header->chromosomeTreeOffset);
  if(!in.good()){
    errorString = "Unable to seek to beginning of chromosomeTree";
    return;
  }
  chromTree = new ChromosomeBTree(in);
  if(chromTree->error()){
    errorString = "Unable to create a ChromosomeBTree";
    return;
  }
  // make the chromosome maps
  std::vector<ChromInfo> c_info;
  chromTree->leafNodes(c_info);
  if(!c_info.size()){
    warnings.push_back("Obtained 0 length chromosome nodes");
  }
  for(unsigned int i=0; i < c_info.size(); ++i){
    chromosomes.insert(std::make_pair(c_info[i].chromId, c_info[i]));
    chrom_NameIndex.insert(std::make_pair(c_info[i].chromName, c_info[i].chromId));
  }
  
  in.seekg(header->fullIndexOffset);
  if(!in.good()){
    errorString = "Unable to seek to full Index offset";
    return;
  }
  rTree = new R_Tree(in);
  if(rTree->error()){
    errorString = "Error in R-Tree";
    return;
  }
  
}

BigFile::~BigFile()
{
  for(unsigned int i=0; i < zoomHeaders.size(); ++i)
    delete zoomHeaders[i];
  delete header;
  delete summary;
  delete chromTree;
  delete rTree;
}

bool BigFile::good(){
  return(!(bool)(errorString.size()));
}

WigSegment* BigFile::segment(std::string chrName, unsigned int beg, unsigned int end)
{
  if(!good())
    return(0);
  if(!chrom_NameIndex.count(chrName))
    return(0);
  
  if(beg > end)
    return(0);

  unsigned int chrId = chrom_NameIndex[chrName];
  if(!chromosomes.count(chrId)){
    std::cerr << "BigFile::segment unknown chrId: " << chrId << std::endl;
    return(0);
  }
  
  std::vector<R_TreeSubNode*> nodes;
  rTree->getLeafNodes(chrId, beg, chrId, end, nodes);
  
  if(!nodes.size()){
    std::cerr << "BigFile::segment Unable to obtain nodes for region "
	      << chrName << " : " << beg << " --> " << end << std::endl;
    return(0);
  }

  // FIX ME PLEASE //
  // this is a bit ugly since we can make a WigSegment from either
  // BigWigSegments for BigBedSegments ..  but we need to know which
  bool isBigWig = !header->isBigBed;
  if(isBigWig){
    std::vector<BigWigSegment*> bwSegments;
    for(unsigned int i=0; i < nodes.size(); ++i)
      bwSegments.push_back(new BigWigSegment(in, nodes[i], header));
    WigSegment* ws = new WigSegment(bwSegments, chrId, chrName);
    
    //// Ugly code alert ////
    for(unsigned int i=0; i < bwSegments.size(); ++i)
      delete bwSegments[i];
    return(ws);
  }
  // And even uglier code ///
  std::vector<BigBedSegment*> bedSegments;
  for(unsigned int i=0; i < nodes.size(); ++i)
    bedSegments.push_back(new BigBedSegment(in, nodes[i], header));
  WigSegment* ws = new WigSegment(bedSegments, chrId, chrName);
  for(unsigned int i=0; i < bedSegments.size(); ++i)
    delete bedSegments[i];
  return(ws);
}

std::vector<ChromInfo> BigFile::chromInfo()
{
  std::vector<ChromInfo> ci; // on error return this
  ci.reserve(chromosomes.size());
  for(std::map<unsigned int, ChromInfo>::iterator it=chromosomes.begin();
      it != chromosomes.end(); ++it)
    ci.push_back( (*it).second );

  return(ci);
}

std::string BigFile::autoSqlString()
{
  return(header->autoSqlString);
}

unsigned int BigFile::fieldCount()
{
  return(header->fieldCount);
}

unsigned int BigFile::definedFieldCount()
{
  return(header->definedFieldCount);
}

TotalSummary BigFile::Summary()
{
  // the disadvantage of using a pointer. If pointer is 0
  // not so good to dereference it.
  if(summary)
    return(*summary);

  std::cout << "Warning, no summary defined" << std::endl;
  TotalSummary ts;
  return(ts);
}

void BigFile::printHeaderSummary()
{
  if(header)
    header->print_summary();
}
