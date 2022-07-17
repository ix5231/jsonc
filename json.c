#include "json.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define eputs(msg) fprintf(stderr, "%s\n", msg)
#define eputch(ch) fprintf(stderr, "%c\n", ch)
#define UNIMPLEMENTED() eputs("UNIMPLEMENTED")
#define expect(expected, found)                                                \
  fprintf(stderr, "[L%d] Expected %s but found %c (%d)\n", __LINE__, expected, \
          found, found)

#define JSON_OBJECT_INITIAL_LEN 2
#define JSON_RESULT_OK 1
#define JSON_RESULT_ERR 1

static json *json_make(void);
static struct json_object *json_object_make(void);
static void json_object_destroy(struct json_object *target);
static int json_object_entry_name_hash(const char *target);
static const char *parse_object(struct json_object *obj, const char *json_str);
static const char *parse_item(struct json_object *obj, const char *json_str);
static const char *parse_integer(const char *json_str, int *res);
static const char *read_token(const char *json_str, const char expects);
static const char *feed_ws(const char *json_str);
static int json_object_insert_item(struct json_object *obj, char *key,
                                   void *data, enum json_item_type type);
static int json_object_grow(struct json_object *obj);

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
  new->root_obj = json_object_make();
  if (!new->root_obj) {
    free(new);
    eputs("json_object allocation failed");
    return NULL;
  }

  return new;
}

static struct json_object *json_object_make(void) {
  struct json_object *new = malloc(sizeof(struct json_object));
  if (!new) {
    eputs("json_object allocation failed");
    return NULL;
  }

  new->size = JSON_OBJECT_INITIAL_LEN;
  new->items = malloc(sizeof(json_item) * new->size);
  if (!new->items) {
    free(new);
    eputs("json_object members allocation failed");
    return NULL;
  }
  for (int i = 0; i < new->size; i++) {
    new->items[i].type = JSON_ITEM_UNINITIALIZED;
    new->items[i].key = NULL;
    new->items[i].data = NULL;
  }

  return new;
}

const json_item *json_get_item(const json *target, const char *key) {
  const int hash = json_object_entry_name_hash(key);
  if (target->root_obj->items[hash].type == JSON_ITEM_UNINITIALIZED) {
    return NULL;
  }
  json_item *candidate = &target->root_obj->items[hash];
  if (strcmp(key, candidate->key) != 0) {
    return NULL;
  }
  return candidate;
}

void json_destroy(json *target) {
  json_object_destroy(target->root_obj);
  free(target);
}

static void json_object_destroy(struct json_object *target) {
  for (int i = 0; i < target->size; i++) {
    if (target->items[i].type != JSON_ITEM_UNINITIALIZED) {
      free(target->items[i].key);
      free(target->items[i].data);
    }
  }
  free(target->items);
  free(target);
};

static const char *parse_object(struct json_object *obj, const char *json_str) {
  const char *next = json_str;
  if (!(next = read_token(next, '{'))) {
    return NULL;
  }

  while (1) {
    next = feed_ws(next);
    switch (*next) {
      case ',':
        next++;
        // dont break;
      case '"':
        next = parse_item(obj, next);
        if (!next) {
          return NULL;
        }
        break;
      case '}':
        next = feed_ws(next);
        return next;
      default:
        expect("} or ,", *next);
        return NULL;
    }
  }
}

static const char *parse_item(struct json_object *obj, const char *json_str) {
  const char *next = json_str;
  if (!(next = read_token(next, '"'))) {
    return NULL;
  }

  int key_len = 0;
  const char *key_start = next;
  while (*next != '"') {
    if (!*next) {
      eputs("Unexpected EOL");
      return NULL;
    }
    next++;
    key_len++;
  }
  next++;

  if (!(next = read_token(next, ':'))) {
    return NULL;
  }

  next = feed_ws(next);
  int *data = malloc(sizeof(int));
  next = parse_integer(next, data);

  char *key = strndup(key_start, key_len);
  if (!key) {
    eputs("Failed to allocate key");
    return NULL;
  }
  if (json_object_insert_item(obj, key, data, JSON_TYPE_INTEGER)) {
    return NULL;
  }

  return next;
}

static const char *parse_integer(const char *json_str, int *res) {
  const char *next = json_str;
  if (!isdigit(*next)) {
    expect("DIGIT", *next);
    return NULL;
  }
  *res = 0;
  while (isdigit(*next)) {
    *res *= 10;
    *res += *next - '0';
    next++;
  }
  return next;
}

static const char *read_token(const char *json_str, const char expects) {
  const char *next = json_str;
  next = feed_ws(next);
  if (*next != expects) {
    char b[2] = {expects, '\0'};
    expect(b, *next);
    return NULL;
  }
  next++;
  return next;
}

static const char *feed_ws(const char *json_str) {
  const char *next = json_str;
  while (*next == ' ' || *next == '\t' || *next == '\n' || *next == '\r') {
    next++;
  }
  return next;
}

static int json_object_insert_item(struct json_object *obj, char *key,
                                   void *data, enum json_item_type type) {
  const int hash = json_object_entry_name_hash(key);
  int rehashed = hash;
  if (obj->items[rehashed].type != JSON_ITEM_UNINITIALIZED) {
    rehashed = (rehashed + 1) % obj->size;
    while (obj->items[rehashed].type != JSON_ITEM_UNINITIALIZED &&
           rehashed != hash)
      ;
    if (rehashed == hash) {
      if (json_object_grow(obj) == JSON_RESULT_ERR) {
        eputs("Failed to insert item");
        return 1;
      }
    }
  }

  obj->items[rehashed].key = key;
  obj->items[rehashed].type = JSON_TYPE_INTEGER;
  obj->items[rehashed].data = data;

  return 0;
}

static int json_object_grow(struct json_object *obj) {
  UNIMPLEMENTED();
  return JSON_RESULT_ERR;
}

static int json_object_entry_name_hash(const char *target) {
  const size_t len = strlen(target);
  int ret = 0;
  for (int i = 0; i < len; i++) {
    ret = (ret + target[i]) % JSON_OBJECT_INITIAL_LEN;
  }
  return ret;
}