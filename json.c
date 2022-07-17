#include "json.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define eputs(msg) fprintf(stderr, "%s\n", msg)
#define UNIMPLEMENTED() eputs("UNIMPLEMENTED")
#define expect(expected, found)                                                \
  fprintf(stderr, "[L%d] Expected %c but found %c (%d)\n", __LINE__, expected, \
          found, found)

static json *json_make(void);
static int json_object_entry_name_hash(const char *target);
static const char *parse_object(struct json_object *obj, const char *json_str);
static const char *parse_item(struct json_object *obj, const char *json_str);

json *json_parse_from(const char *json_str) {
  json *new = json_make();
  if (!new) {
    return NULL;
  }

  if (!parse_object(new->root_obj, json_str)) {
    eputs("Parse failed");
    json_destroy(new);
    return NULL;
  }

  return new;
}

static json *json_make(void) {
  json *new = malloc(sizeof(json));
  if (!new) {
    eputs("json allocation failed");
    return NULL;
  }
  new->root_obj = malloc(sizeof(struct json_object));
  if (!new->root_obj) {
    free(new);
    eputs("json_object allocation failed");
    return NULL;
  }

  for (int i = 0; i < JSON_OBJECT_LEN; i++) {
    new->root_obj->items[i].type = JSON_ITEM_UNINITIALIZED;
    new->root_obj->items[i].data = NULL;
  }
  return new;
}

const json_item *json_get_item(const json *target, const char *entry) {
  const int hash = json_object_entry_name_hash(entry);
  if (target->root_obj->items[hash].type == JSON_ITEM_UNINITIALIZED) {
    return NULL;
  }
  return &target->root_obj->items[hash];
}

void json_destroy(json *target) {
  for (int i = 0; i < JSON_OBJECT_LEN; i++) {
    if (target->root_obj->items[i].type != JSON_ITEM_UNINITIALIZED) {
      free(target->root_obj->items[i].data);
    }
  }
  free(target->root_obj);
  free(target);
}

static const char *parse_object(struct json_object *obj, const char *json_str) {
  const char *next = json_str;
  if (*next != '{') {
    expect('{', *next);
    return NULL;
  }
  next++;

  next = parse_item(obj, next);
  if (!next) {
    return NULL;
  }

  if (*next != '}') {
    expect('}', *next);
    return NULL;
  }
  next++;

  return next;
}

static const char *parse_item(struct json_object *obj, const char *json_str) {
  const char *next = json_str;
  if (*next != '"') {
    expect('\"', *next);
    return NULL;
  }
  next++;

  int hash = 0;
  while (*next != '"') {
    if (!*next) {
      eputs("Unexpected EOL");
      return NULL;
    }
    hash = (hash + *next) % JSON_OBJECT_LEN;
    next++;
  }
  next++;

  if (*next != ':') {
    expect(':', *next);
    return NULL;
  }
  next++;

  if (*next != '1') {
    UNIMPLEMENTED();
    return NULL;
  }
  next++;

  obj->items[hash].type = JSON_TYPE_INTEGER;
  int *data = malloc(sizeof(1));
  *data = 1;
  obj->items[hash].data = data;

  return next;
}

static int json_object_entry_name_hash(const char *target) {
  const size_t len = strlen(target);
  int ret = 0;
  for (int i = 0; i < len; i++) {
    ret = (ret + target[i]) % JSON_OBJECT_LEN;
  }
  return ret;
}