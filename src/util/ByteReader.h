#ifndef BYTEREADER_H
#define BYTEREADER_H

#include <stdlib.h>

/*  An object that converts bytes (unsigned char*) to various
    types of primitives (eg. short, int, float, whatever) by
    casting pointers rathern than primitives

    If reverse bytes is set it will make use of GCC built in functions
    __builtin_bswap16, __builtin_bswap32, __builtin_bswap64
    
    making the code specific to GCC. But what other compiler would you use?

    ByteReader objects take a char* pointer of specified length to read from.
    However, ByteReader does not take ownership of this pointer, and does not
    delete it. This behaviour may be good to change with time.

    Although we ask for the length of the data, it's not actually used at the moment
    There is no error checking.
*/

class ByteReader {
 public:
  ByteReader();
  ByteReader(const char* data_ptr, size_t data_length, bool reverse_bytes);
  
  ~ByteReader(){};
  
  void set_data(const char* data_ptr, size_t data_length, bool reverse_bytes);

  // these take an offset and return the specified type
  short s(size_t o);
  unsigned short us(size_t o);
  int i(size_t o);
  unsigned int ui(size_t o);
  long l(size_t o);
  unsigned long ul(size_t o);
  
  float f(size_t o);
  double d(size_t o);

 private:
  const char* data;
  size_t d_length;
  bool reverse;


};


#endif
