#ifndef FILE_H
#define FILE_H
#include "obj.h"

void file_init();
bool is_file(obj o);
obj file_open(obj s, obj m);
obj file_readline(obj f);
obj file_lines(obj f);

#endif

#ifdef FILE_IMPLEMENTATION

#include <stdio.h>

void file_free(obj o) {
  FILE ** f = o;
  fclose(*f);
}

void file_print(obj o) {
  FILE ** f = o;
  printf("file");
}

obj file_str(obj s) {
  auto_begin();
  obj r = str_c("file");
  auto_end_return(r);
}

int file_cmpi(const void * a, const void * b) {
  return (*(obj *)a) - (*(obj *)b);
}

static int file_type_id;
static type file_type = {
  .free = file_free,
  .print = file_print,
  .str = file_str,
  .cmpi = file_cmpi,
  .name = "file"
};

void file_init() {
  file_type_id = register_type(&file_type);
}

int is_file(obj o) {
  return obj_type(o) == file_type_id;
}

obj file_open(obj s, obj m) {
  assert(is_str(s) && is_str(m));
  FILE ** ad;
  obj_scope {
    ad = auto_new(sizeof(FILE *), file_type_id);
    *ad = fopen(c_str(s), c_str(m));
    ref(ad);
  }
  obj_return(ad);
}

obj file_readline(obj f) {
  assert(is_file(f));
  FILE ** fp = f;
  char c = fgetc(*fp);
  if(feof(*fp)) {
    ref(nil);
    obj_return(nil);
  }
  obj nd;
  obj_scope {
    nd = str_c("");
    while(c!=EOF) {
      if(c=='\n') break;
      str_put(nd, c);
      c = fgetc(*fp);
    }
    ref(nd);
  }
  obj_return(nd);
}

obj file_lines(obj f) {
  assert(is_file(f));
  FILE ** fp = f;
  fseek(*fp, SEEK_SET, 0);
  obj a;
  obj_scope {
    a = array_new(0, nil);
    while(1) {
      auto_begin();
      obj l = file_readline(f);
      if(is_nil(l)) {
        auto_end();
        break;
      }  
      array_push(a, l);
      auto_end();
    }
    ref(a);
  }
  obj_return(a);
}

#endif
