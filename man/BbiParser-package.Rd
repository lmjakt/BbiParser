\name{BbiParser-package}
\alias{BbiParser-package}
\alias{BbiParser}
\docType{package}
\title{
Provides access to BigBinaryIndexed files
}
\description{
  Provides read access to BigWig and BigBed files.
  In the initial stages of development. Provides one
  function BBI_WigRegion to obtain wig type data from
  a specified file, chromosome and region.
}
\details{
\tabular{ll}{
Package: \tab BbiParser\cr
Type: \tab Package\cr
Version: \tab 1.0\cr
Date: \tab 2013-06-12\cr
License: \tab GPL-2\cr
}
~~ An overview of how to use the package, including the most important ~~
~~ functions ~~
}
\author{
Martin Jakt

Maintainer: Martin Jakt <mjakt@z2.keio.jp>
}
\references{
~~ Literature or other references for background information ~~
}
%%~~ Optionally other standard keywords, one per line, from file KEYWORDS in ~~
%%~~ the R documentation directory ~~
\keyword{ package }
\seealso{
~~ Optional links to other man pages, e.g. ~~
%%~~ \code{\link[<pkg>:<pkg>-package]{<pkg>}} ~~
}
\examples{
%% ~~ simple examples of the most important functions ~~
\dontrun{
rcpp_hello_world()
BBI_WigRegion(fileName, chrName, chrStart, chrEnd)
}
}
