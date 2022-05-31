#ifndef STR_H
#define STR_H
#include "obj.h"

void str_init();
int is_str(obj o);
obj str_new(size_t n, char * s);
obj str_c(char * s);
char * c_str(obj o);
size_t str_len(obj o);
obj str_cat(obj a, obj b);
int str_cmp(obj a, obj b);
void str_push(obj a, obj b);
void str_put(obj a, char c);
obj str_split(obj a, obj b);
obj str_join(obj a, obj b);

#endif

#ifdef STR_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>
#include "array.h"

typedef struct str_data {
  size_t capacity;
  size_t len;
  obj data;
} str_data;

void str_free(obj s) {
  str_data * sd = s;
  unref(sd->data);
}

void str_print(obj s) {
  str_data * sd = s;
  char * d = (char *)sd->data;
  putc('"', stdout);
  for(int i=0; i<sd->len; ++i) {
    putc(d[i], stdout);
  }
  putc('"', stdout);
}

obj str_str(obj s) {
  auto_begin();
  obj r = str_c("");
  str_push(r, str_c("\""));
  str_push(r, s);
  str_push(r, str_c("\""));
  auto_end_return(r);
}

int str_cmpi(const void * a, const void * b) {
  return str_cmp(*(obj *)a, *(obj *)b);
}

static int str_type_id;
static type str_type = {
  .free = str_free,
  .print = str_print,
  .str = str_str,
  .cmpi = str_cmpi,
  .name = "str"
};

void str_init() {
  str_type_id = register_type(&str_type);
}

int is_str(obj o) {
  return obj_type(o) == str_type_id;
}

obj str_new(size_t n, char * s) {
  auto_begin();
  str_data * sd = auto_new(sizeof(str_data), str_type_id);
  sd->len = n;
  sd->capacity = size_pow2(n+1);
  sd->data = raw_new(sd->capacity);
  ref(sd->data);
  char * sp = (char *)sd->data;
  memcpy(sp, s, sd->len);
  sp[sd->len] = 0;
  auto_end_return(sd);
}

obj str_c(char * s) {
  auto_begin();
  obj o = str_new(strlen(s), s);
  auto_end_return(o);
}

size_t str_len(obj s) {
  assert(is_str(s));
  str_data * sd = s;
  return sd->len;
}

char * c_str(obj s) {
  assert(is_str(s));
  str_data * sd = s;
  return (char *)sd->data;
}

obj str_cat(obj sa, obj sb) {
  assert(is_str(sa) && is_str(sb));
  auto_begin();
  obj r = str_c("");
  str_push(r, sa);
  str_push(r, sb);
  auto_end_return(r);
}

int str_cmp(obj a, obj b) {
  return strcmp(c_str(a), c_str(b));
}

void str_put(obj s, char c) {
  assert(is_str(s));
  auto_begin();
  str_data * sd = s;
  int nlen = sd->len+1;
  if(nlen+1>sd->capacity) {
    int nc = size_pow2(nlen+1);
    obj nd = raw_new(nc);
    memcpy(nd, sd->data, sd->len);
    unref(sd->data);
    sd->capacity = nc;
    sd->data = nd; ref(nd);
  } 
  char * d = sd->data;
  d[sd->len] = c;
  d[sd->len+1] = 0;
  sd->len = nlen;
  auto_end();
}

void str_push(obj sa, obj sb) {
  assert(is_str(sa) && is_str(sb));
  auto_begin();
  str_data * ad = sa;
  str_data * bd = sb;
  int nlen = ad->len+bd->len;
  if(nlen+1>ad->capacity) {
    int nc = size_pow2(nlen+1);
    obj nd = raw_new(nc);
    memcpy(nd, ad->data, ad->len);
    unref(ad->data);
    ad->capacity = nc;
    ad->data = nd; ref(nd);
  } 
  char * d = ad->data;
  strcpy(&d[ad->len], (char *)bd->data);
  ad->len = nlen;
  auto_end();
}

obj str_split(obj sa, obj sb) {
  assert(is_str(sa) && is_str(sb));
  auto_begin();
  obj a = array_new(0, nil);
  char * pa = c_str(sa);
  char * pb = c_str(sb);
  char * i = pa;
  char * j; 
  while(j=strstr(i, pb)) {
    auto_begin();
    array_push(a, str_new(j-i, i));
    i = j + str_len(sb);
    auto_end();
  }
  j = pa + str_len(sa);
  if(i != j)
    array_push(a, str_new(j-i, i));
  auto_end_return(a);
}

obj str_join(obj a, obj s) {
  assert(is_str(s));
  auto_begin();
  obj r = str_c("");
  for(int i=0; i<array_len(a); ++i) {
    auto_begin();
    if(i) str_push(r, s);
    str_push(r, array_get(a, i));
    auto_end();
  }
  auto_end_return(r);
}

#endif
