#ifndef OBJ_H
#define OBJ_H

#include "types.h"

typedef void * obj;
typedef void (*method)(obj);
typedef obj (*method1)(obj);
typedef int (*method_cmpi)(const void *, const void *);
typedef struct type {
  method free;
  method print;
  method1 str;
  method_cmpi cmpi; 
  const char * name;
} type;

void obj_init();
void obj_final();
void obj_print(obj o);
void obj_println(obj o);
obj str_obj(obj o);
int obj_type(obj o);
int obj_cmpi(const void * a, const void * b);
int register_type(const type * t);
obj heap_new(size_t n, int type);
obj auto_new(size_t n, int type);
void auto_push(obj o);
obj auto_pop();
void auto_begin();
void auto_end();
#define scope defer(auto_begin(),auto_end())
#define auto_end_return(o) ref(o); auto_end(); auto_push(o); return o
#define obj_return(o) auto_push(o); return o
void ref(obj o);
void unref(obj o);
bool is_nil(obj o);
extern obj nil;
bool is_raw(obj o);
obj raw_new(size_t n);

#endif

#ifdef OBJ_IMPLEMENTATION

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

typedef struct obj_data {
  int32_t count;
  int32_t type;
} obj_data;

#define MAX_TYPES 1024
static const type * types[MAX_TYPES];
static int types_count = 0;

#define MAX_STACK 1024
static obj stack[MAX_STACK];
static int stack_count = 0;

#define MAX_FRAMES 256
static int frames[MAX_FRAMES];
static int frames_count = 0;

int register_type(const type * t) {
  assert(types_count<MAX_TYPES);
  types[types_count++] = t;
  return types_count-1;
}

static int nil_type_id;
void nil_print(obj o) {
  printf("nil");
}

obj nil_str(obj o) {
  obj r;
  scope { 
    r = str_c("nil");
    ref(r);
  }
  obj_return(r);
}

int nil_cmpi(const void * a, const void * b) {
  return 0;
}

static type nil_type = {
  .free = 0,
  .print = nil_print,
  .str = nil_str,
  .cmpi = nil_cmpi,
  .name = "nil"
};

obj nil;

static int raw_type_id;
void raw_print(obj o) {
  printf("%p", o);
}

obj raw_str(obj o) {
  static char buf[256];
  sprintf(&buf[0], "%p", o);
  obj r;
  scope {
    r = str_c(&buf[0]);
    ref(r);
  }
  obj_return(r);
}

int raw_cmpi(const void * a, const void * b) {
  return (*(obj *)a)-(*(obj *)b);
}

static type raw_type = {
  .free = 0,
  .print = raw_print,
  .str = raw_str,
  .cmpi = raw_cmpi,
  .name = "raw"
};

static int total_refs = 0;
static int total_unrefs = 0;

void obj_init() {
  nil_type_id = register_type(&nil_type);
  raw_type_id = register_type(&raw_type);
  nil = heap_new(0, nil_type_id);
}

bool is_nil(obj o) {
  return obj_type(o) == nil_type_id;
}

void obj_final() {
  unref(nil);
  printf("refs  : %d\n", total_refs);
  printf("unrefs: %d\n", total_unrefs);
}

int obj_type(obj o) {
  obj_data * od = ((obj_data *)o)-1;
  return (int)od->type;
}

int obj_cmpi(const void * a, const void * b) {
  int ta = obj_type(*(obj *)a);
  int tb = obj_type(*(obj *)b);
  if(ta == tb) 
    return types[ta]->cmpi(a, b);
  return ta-tb;
}

void obj_print(obj o) {
  types[obj_type(o)]->print(o);
}

obj str_obj(obj o) {
  obj r;
  scope {
    r = types[obj_type(o)]->str(o);
    ref(r);
  }
  obj_return(r);
}

void obj_println(obj o) {
  obj_print(o);
  printf("\n");
}

void auto_push(obj o) {
  assert(stack_count<MAX_STACK);
  stack[stack_count++] = o;
}

obj auto_pop() {
  assert(stack_count>0);
  return stack[--stack_count];
}

void auto_begin() {
  assert(frames_count<MAX_FRAMES);
  frames[frames_count++] = stack_count;
}

void auto_end() {
  assert(frames_count>0);
  frames_count--;
  while(stack_count>frames[frames_count]) 
    unref(auto_pop());
}

void ref(obj o) {
  obj_data * obj_ptr = ((obj_data *)o)-1;
  obj_ptr->count++;
  total_refs++;
}

void unref(obj o) {
  obj_data * obj_ptr = ((obj_data *)o)-1;
  assert(obj_ptr->count>0);
  obj_ptr->count--;
  if(obj_ptr->count==0) {
    const type * t = types[obj_ptr->type];
    if(t->free) t->free(o);
    free(obj_ptr);
  }
  total_unrefs++;
}

obj heap_new(size_t n, int type) {
  obj_data * obj_ptr = (obj_data *)malloc(sizeof(obj_data)+n);
  obj o = (obj)(obj_ptr+1);
  obj_ptr->count = 0;
  obj_ptr->type = (int32_t)type;
  ref(o);
  return o;
}

obj auto_new(size_t n, int type) {
  obj o = heap_new(n, type);
  auto_push(o);
  return o;
}

obj raw_new(size_t n) {
  obj o;
  scope {
    o = auto_new(n, raw_type_id);
    ref(o);
  }
  obj_return(o);
}

int is_raw(obj o) {
  return obj_type(o) == raw_type_id;
}

#endif
