TEMPLATE = app
CONFIG = warn_on debug
LIBS	    += -lz -lm
HEADERS = util/ByteReader.h \
	  read_big/BigHeader.h \
	  read_big/ZoomHeader.h \
	  read_big/TotalSummary.h \
	  read_big/ChromosomeBTree.h \
	  read_big/ChromosomeNode.h \
	  read_big/R_Tree.h \
	  read_big/R_TreeNode.h \
	  read_big/BigWigSegment.h \
	  read_big/DataSegment.h \
	  read_big/BigFile.h \
	  export_structures/WigSegment.h 
SOURCES = util/ByteReader.cpp \
	  read_big/BigHeader.cpp \
	  read_big/ZoomHeader.cpp \
	  read_big/TotalSummary.cpp \
	  read_big/ChromosomeBTree.cpp \
	  read_big/ChromosomeNode.cpp \
	  read_big/R_Tree.cpp \
	  read_big/R_TreeNode.h \
	  read_big/BigWigSegment.cpp \
	  read_big/DataSegment.cpp \
	  read_big/BigFile.cpp \
	  export_structures/WigSegment.cpp \
	  browse_header_information.cpp
TARGET = big_reader
