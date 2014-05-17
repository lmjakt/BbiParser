#ifndef BYTEREADER_H
#define BYTEREADER_H

#include <stdlib.h>
#include <string>

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
  enum ReadState { GOOD=0, END=1, NO_BUFFER=2 };

  ByteReader();
  ByteReader(const char* data_ptr, size_t data_length, bool reverse_bytes);
  
  ~ByteReader(){};
  
  void set_data(const char* data_ptr, size_t data_length, bool reverse_bytes);
  
  ReadState readState(){ return rState; }
  
  // these take an offset and return the specified type
  // without an offset they act in a stream like manner, reading
  // from the current_offset and incrementing it afterwards.
  // on failure the rState is set
  short s(size_t o);
  short s();

  unsigned short us(size_t o);
  unsigned short us();
  
  int i(size_t o);
  int i();

  unsigned int ui(size_t o);
  unsigned int ui();

  long l(size_t o);
  long l();

  unsigned long ul(size_t o);
  unsigned long ul();
  
  float f(size_t o);
  float f();

  double d(size_t o);
  double d();

  // read a c_string from the data into a std::string
  // does not include the 0 but
  // if no 0 sets terminated to false
  std::string c_str(bool& terminated);

 private:
  const char* data;
  size_t d_length;
  bool reverse;
  
  ReadState rState;
  size_t currentPos;

  // returns a pointer to the dst.
  char* read_number(char* dst, size_t length);
  char* read_number(char* dst, size_t length, size_t offset);

  
};


#endif
