loadModule("mod_RcppBigFile", TRUE)

rcpp_hello_world <- function(){
	.Call( "rcpp_hello_world", PACKAGE = "BbiParser" )
}

BBI_WigRegion <- function(fileName, chrName, chrStart, chrEnd){
  .Call("Bbi_get_wig_region", fileName, chrName, chrStart, chrEnd, PACKAGE="BbiParser")
}

BBI_Smooth_Wig <- function(fileNames, chrName, chrPos, con_kernel){
  .Call("Bbi_get_wig_smoothened_regions", fileNames, chrName, chrPos, con_kernel,
        PACKAGE="BbiParser")
}
