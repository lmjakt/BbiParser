#ifndef _BbiParser_BBIPARSER_H
#define _BbiParser_BBIPARSER_H

#include <Rcpp.h>

RcppExport SEXP Bbi_get_wig_smoothened_regions(SEXP file_names, SEXP chrId, SEXP chrPos, SEXP con_kernel);

RcppExport SEXP Bbi_get_wig_region(SEXP file_name, SEXP chr_name, SEXP start, SEXP end);

#endif
