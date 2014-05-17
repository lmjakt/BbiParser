#include "ByteReader.h"
#include <string.h>
#include <byteswap.h>

// byteswap.h included since there appears to be no
// __builtin_swapb16

ByteReader::ByteReader()
{
  data = 0;
  d_length = 0;
  reverse = false;
  currentPos = 0;
  rState = NO_BUFFER;
}

ByteReader::ByteReader(const char* data_ptr, size_t data_length, bool reverse_bytes)
{
  data = data_ptr;
  d_length = data_length;
  reverse = reverse_bytes;
  currentPos = 0;
  rState = data ? GOOD : NO_BUFFER;
}

void ByteReader::set_data(const char* data_ptr, size_t data_length, bool reverse_bytes)
{
  data = data_ptr;
  d_length = data_length;
  reverse = reverse_bytes;
  currentPos = 0;
  rState = data ? GOOD : NO_BUFFER;
}

short ByteReader::s(size_t o)
{
  if(!reverse)
    return( *(short*)(data + o) );
  return( __bswap_16(*(short*)(data + o)) );
}

short ByteReader::s()
{
  short v = 0;
  return( *(short*)read_number( (char*)&v, sizeof(short)));
}
    
unsigned short ByteReader::us(size_t o)
{
  if(!reverse)
    return( *(unsigned short*)(data + o) );
  return( __bswap_16(*(unsigned short*)(data + o)) );
}

unsigned short ByteReader::us()
{
  unsigned short v = 0;
  return( *(unsigned short*)read_number( (char*)&v, sizeof(unsigned short) ));
}

int ByteReader::i(size_t o)
{
  if(!reverse)
    return( *(int*)(data + o) );
  return( __builtin_bswap32( *(int*)(data + o)) );
}

int ByteReader::i()
{
  int v = 0;
  return( *(int*)read_number( (char*)&v, sizeof(int) ));
}

unsigned int ByteReader::ui(size_t o)
{
  if(!reverse)
    return( *(unsigned int*)(data + o) );
  return( __builtin_bswap32( *(unsigned int*)(data + o)) );
}

unsigned int ByteReader::ui()
{
  unsigned int v = 0;
  return( *(unsigned int*)read_number( (char*)&v, sizeof(unsigned int) ));
}

long ByteReader::l(size_t o)
{
  if(!reverse)
    return( *(long*)(data + o) );
  return( __builtin_bswap64( *(long*)(data + o)) );
}

long ByteReader::l()
{
  long v = 0;
  return( *(long*)read_number((char*)&v, sizeof(long) ));
}

unsigned long ByteReader::ul(size_t o)
{
  if(!reverse)
    return( *(unsigned long*)(data + o) );
  return( __builtin_bswap64( *(unsigned long*)(data + o)) );
}

unsigned long ByteReader::ul()
{
  unsigned long v=0;
  return( *(long*)read_number((char*)&v, sizeof(unsigned long) ));
}

float ByteReader::f(size_t o)
{
  if(!reverse)
    return( *(float*)(data + o) );
  return( __builtin_bswap32( *(float*)(data + o)) );
}

float ByteReader::f()
{
  float v=0;
  return( *(float*)read_number((char*)&v, sizeof(float)) );
}

double ByteReader::d(size_t o)
{
  if(!reverse)
    return( *(double*)(data + o) );
  return( __builtin_bswap32( *(double*)(data + o)) );
}

double ByteReader::d()
{
  double v=0;
  return( *(float*)read_number((char*)&v, sizeof(double)) );
}

// this would probably be better as a template function
char* ByteReader::read_number(char* dst, size_t length)
{
  return( read_number(dst, length, currentPos) );
}

char* ByteReader::read_number(char* dst, size_t length, size_t offset)
{
  if( length + offset > d_length ){
    memset(dst, 0, length);
    rState = END;
    return(dst);
  }
  currentPos = offset + length;
  const char* src = data + offset; 
  if(!reverse || length == 1){
    memcpy(dst, (void*)(src), length);
    return(dst);
  }
  // we can use the built in functions here, but not sure how much that
  switch(length){
  case 2:
    *(unsigned short*)dst = __bswap_16(*(unsigned short*)src);
    break;
  case 4:
    *(unsigned int*)dst = __builtin_bswap32(*(unsigned int*)src);
    break;
  case 8:
    *(unsigned long*)dst = __builtin_bswap64(*(unsigned long*)src);
    break;
  default:
    for(unsigned int i=0;i < length; ++i)
      dst[i] = src[ length-(i+1) ];
  }
  return(dst);
}

std::string ByteReader::c_str(bool& terminated){
  terminated = false;
  if(currentPos >= d_length){
    rState = END;
    return( std::string() );
  }
  const char* src_beg = data + currentPos;
  const char* src_end = src_beg;
  const char* end = data + d_length;
  while( src_end < end && *src_end != 0 )
    ++src_end;

  if(src_end < end){  // i.e. we found a 0
    terminated = true;
    ++src_end; // not at end, need to move up
  }

  currentPos = (src_end - data);
  rState = currentPos < d_length ? GOOD : END;
  return( std::string(src_beg, (src_end-src_beg)) );
  
}
