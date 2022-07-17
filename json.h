#ifndef JSON_H
#define JSON_H

enum json_item_type {
  JSON_ITEM_UNINITIALIZED,
  JSON_TYPE_INTEGER,
};

struct json_item {
  enum json_item_type type;
  char *key;
  void *data;
};
typedef struct json_item json_item;

struct json_object {
  int size;
  struct json_item *items;
};

struct json {
  struct json_object *root_obj;
};
typedef struct json json;

void json_destroy(json *target);
json *json_parse_from(const char *json_str);
const json_item *json_get_item(const json *target, const char *key);

#endif