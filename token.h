#ifndef TOKEN_H
#define TOKEN_H

#include <stddef.h>

typedef struct utf8_s {
  int size;
  unsigned int utf
}   

#endif

#ifdef TOKEN_IMPLEMENTATION

utf8_s tok_utf8(char * s) {
  char c = *s;
  if(c&0x80)
  char * spc = " \t\r\n";
  for(int i=0;i<4;++i) 
    if(c==spc[i]) return 1;
  return 0;
}

#endif 