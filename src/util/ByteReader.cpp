#include "ByteReader.h"
#include <byteswap.h>

// byteswap.h included since there appears to be no
// __builtin_swapb16

ByteReader::ByteReader()
{
  data = 0;
  d_length = 0;
  reverse = false;
}

ByteReader::ByteReader(const char* data_ptr, size_t data_length, bool reverse_bytes)
{
  data = data_ptr;
  d_length = data_length;
  reverse = reverse_bytes;
}

void ByteReader::set_data(const char* data_ptr, size_t data_length, bool reverse_bytes)
{
  data = data_ptr;
  d_length = data_length;
  reverse = reverse_bytes;
}

short ByteReader::s(size_t o)
{
  if(!reverse)
    return( *(short*)(data + o) );
  return( __bswap_16(*(short*)(data + o)) );
}

unsigned short ByteReader::us(size_t o)
{
  if(!reverse)
    return( *(unsigned short*)(data + o) );
  return( __bswap_16(*(unsigned short*)(data + o)) );
}

int ByteReader::i(size_t o)
{
  if(!reverse)
    return( *(int*)(data + o) );
  return( __builtin_bswap32( *(int*)(data + o)) );
}

unsigned int ByteReader::ui(size_t o)
{
  if(!reverse)
    return( *(unsigned int*)(data + o) );
  return( __builtin_bswap32( *(unsigned int*)(data + o)) );
}

long ByteReader::l(size_t o)
{
  if(!reverse)
    return( *(long*)(data + o) );
  return( __builtin_bswap64( *(long*)(data + o)) );
}

unsigned long ByteReader::ul(size_t o)
{
  if(!reverse)
    return( *(unsigned long*)(data + o) );
  return( __builtin_bswap64( *(unsigned long*)(data + o)) );
}

float ByteReader::f(size_t o)
{
  if(!reverse)
    return( *(float*)(data + o) );
  return( __builtin_bswap32( *(float*)(data + o)) );
}

double ByteReader::d(size_t o)
{
  if(!reverse)
    return( *(double*)(data + o) );
  return( __builtin_bswap32( *(double*)(data + o)) );
}

