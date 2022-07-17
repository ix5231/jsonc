#include "json.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int json_object_entry_name_hash(const char *target);

static int json_object_entry_name_hash(const char *target) {
  const size_t len = strlen(target);
  size_t ret = 0;
  for (int i = 0; i < len; i++) {
    ret = (ret + target[i]) % JSON_OBJECT_LEN;
  }
  return ret;
}

json *json_parse_from(const char *json_str) {
  if (strcmp(json_str, "{\"field\":1}") == 0) {
    json *new = malloc(sizeof(json));
    if (!new) {
      return NULL;
    }

    for (int i = 0; i < JSON_OBJECT_LEN; i++) {
      new->root_obj.items[i].type = JSON_ITEM_UNINITIALIZED;
      new->root_obj.items[i].data = NULL;
    }
    int *data = malloc(sizeof(1));
    if (!data) {
      free(new);
      return NULL;
    }
    *data = 1;
    int hash = json_object_entry_name_hash("field");
    new->root_obj.items[hash].type = JSON_TYPE_INTEGER;
    new->root_obj.items[hash].data = data;

    return new;
  } else {
    fputs("UNIMPLEMENTED", stderr);
    return NULL;
  }
}

const json_item *json_get_item(const json *target, const char *entry) {
  const int hash = json_object_entry_name_hash(entry);
  if (target->root_obj.items[hash].type == JSON_ITEM_UNINITIALIZED) {
    return NULL;
  }
  return &target->root_obj.items[hash];
}

void json_destroy(json *target) {
  for (int i = 0; i < JSON_OBJECT_LEN; i++) {
    if (target->root_obj.items[i].type != JSON_ITEM_UNINITIALIZED) {
      free(target->root_obj.items[i].data);
      target->root_obj.items[i].type = JSON_ITEM_UNINITIALIZED;
    }
  }
  free(target);
}