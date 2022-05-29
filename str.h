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
  str_data * ad = s;
  unref(ad->data);
}

void str_print(obj s) {
  str_data * ad = s;
  char * d = (char *)ad->data;
  for(int i=0; i<ad->len; ++i) {
    putc(d[i], stdout);
  }
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
  str_data * ad = auto_new(sizeof(str_data), str_type_id);
  ad->len = n;
  ad->capacity = size_pow2(n+1);
  ad->data = raw_new(ad->capacity);
  ref(ad->data);
  char * sd = (char *)ad->data;
  memcpy(sd, s, ad->len);
  sd[ad->len] = 0;
  auto_end_return(ad);
}

obj str_c(char * s) {
  auto_begin();
  obj o = str_new(strlen(s), s);
  auto_end_return(o);
}

size_t str_len(obj s) {
  assert(is_str(s));
  str_data * ad = s;
  return ad->len;
}

char * c_str(obj s) {
  assert(is_str(s));
  str_data * ad = s;
  return (char *)ad->data;
}

obj str_cat(obj sa, obj sb) {
  assert(is_str(sa) && is_str(sb));
  auto_begin();
  str_data * ad = sa;
  str_data * bd = sb;
  str_data * nd = auto_new(sizeof(str_data), str_type_id);
  nd->len = ad->len+bd->len;
  nd->capacity = size_pow2(nd->len+1);
  nd->data = raw_new(nd->capacity);
  ref(nd->data);
  char * s = (char *)nd->data;
  memcpy(s, (char *)ad->data, ad->len);
  memcpy(s+ad->len, (char *)bd->data, bd->len+1);
  auto_end_return(nd);
}

int str_cmp(obj a, obj b) {
  return strcmp(c_str(a), c_str(b));
}

void str_put(obj sa, char c) {
  assert(is_str(sa));
  auto_begin();
  str_data * ad = sa;
  int nlen = ad->len+1;
  if(nlen+1>ad->capacity) {
    int nc = size_pow2(nlen+1);
    obj nd = raw_new(nc);
    memcpy(nd, ad->data, ad->len);
    unref(ad->data);
    ad->capacity = nc;
    ad->data = nd; ref(nd);
  } 
  char * d = ad->data;
  d[ad->len] = c;
  d[ad->len+1] = 0;
  ad->len = nlen;
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
