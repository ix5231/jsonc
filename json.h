#ifndef JSON_H
#define JSON_H

#define JSON_OBJECT_LEN 23

enum json_item_type {
  JSON_ITEM_UNINITIALIZED,
  JSON_TYPE_INTEGER,
};

struct json_item {
  enum json_item_type type;
  void *data;
};
typedef struct json_item json_item;

struct json_object {
  struct json_item items[JSON_OBJECT_LEN];
};

struct json {
  struct json_object *root_obj;
};
typedef struct json json;

void json_destroy(json *target);
json *json_parse_from(const char *json_str);
const json_item *json_get_item(const json *target, const char *entry);

#endif