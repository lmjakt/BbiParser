#include "BbiParser.h"
#include "read_big/BigFile.h"
#include "export_structures/WigSegment.h"
#include "R_Cpp/util_functions.h"
#include <string>
#include <vector>
#include <map>


SEXP Bbi_get_wig_region(SEXP file_name, SEXP chr_name, SEXP start, SEXP end)
{
  std::string fname = Rcpp::as<std::string>(file_name); // explicit
  std::string chrome = Rcpp::as<std::string>(chr_name); // implicit ? which ok
  unsigned int c_start = Rcpp::as<unsigned int>(start);
  unsigned int c_end = Rcpp::as<unsigned int>(end);
  
  // we are goig to wrap up a std::map<std::string, std::vector<float> >
  // to pass back to R.
  // Unfortunately I'm unable to find the actual specifications for the
  // GenericVector class (it's a template of Vector

  std::vector<unsigned int> starts;
  std::vector<unsigned int> ends;
  std::vector<float> values;
  
  BigFile bf(fname.c_str());
  if(!bf.good())
    return( make_return_data(chrome, starts, ends, values) );
  
  WigSegment* segment = bf.segment(chrome, c_start, c_end);
  if(!segment)
    return( make_return_data(chrome, starts, ends, values) );

  starts = segment->starts();
  ends = segment->ends();
  values = segment->values();
  delete segment;
  return( make_return_data(chrome, starts, ends, values) );
}

SEXP Bbi_get_wig_smoothened_regions(SEXP file_names, SEXP chr_name, 
				    SEXP chrPos, SEXP con_kernel)
{
  std::vector<std::string> fnames = Rcpp::as<std::vector<std::string> >(file_names);
  std::string chrome = Rcpp::as<std::string>(chr_name);
  std::vector<int> pos = Rcpp::as<std::vector<int> >(chrPos);
  std::vector<float> kernel = Rcpp::as<std::vector<float> >(con_kernel);

  // apparently storage is by column, and each file will give chrPos.size()
  // vector of values to stick into the column.
  // the matrix constructor takes row, column sizes
  Rcpp::NumericMatrix mat(pos.size(), fnames.size()); 

  // data for failures.. all 0
  std::vector<float> fail_values(pos.size(), 0);

  for(unsigned int i=0; i < fnames.size(); ++i){
    for(unsigned int j=0; j < pos.size(); ++j)
      mat(j,i) = 0;
    BigFile bf(fnames[i].c_str());
    if(!bf.good()){
      continue;
    }
    WigSegment* segment = bf.segment(chrome, pos.front(), pos.back());
    if(!segment){
      delete segment;
      continue;
    }
    std::vector<float> v = segment->kernel_values(pos, kernel);
    if(v.size() == pos.size()){
      for(unsigned int j=0; j < v.size(); ++j)
	mat(j,i) = v[j];
    }
    delete segment;
  }
  
  return( mat );
}
