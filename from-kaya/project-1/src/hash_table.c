#include "hash_table.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#define OVERFLOW_FACTOR 2

size_t index_in_table(hash_table table, uint64_t hash);

hash_table make_hash_table(size_t entry_count) {
  size_t table_capacity = entry_count * OVERFLOW_FACTOR;

  hash_table table;
  table.data = (hash_entry *)calloc(table_capacity, sizeof(hash_entry));
  table.count = table_capacity;

  return table;
}

void free_hash_table(hash_table *table) {
  free(table->data);
  table->data = NULL;
}

bool insert_into_table(hash_table table, uint64_t hash, uint64_t value) {
  size_t index = index_in_table(table, hash);
  hash_entry *table_entry = &table.data[index];

  size_t total_checked = 0;
  while (table_entry->occupied && total_checked < table.count) {
    table_entry++;

    if (table_entry - table.data >= table.count) {
      table_entry = table.data;
    }

    total_checked++;
  }

  if (total_checked >= table.count) {
    return false;
  }

  table_entry->hash = hash;
  table_entry->nonce = value;
  table_entry->occupied = true;
  return true;
}

bool find_inside_table(hash_table table, uint64_t hash, uint64_t *value) {
  size_t index = index_in_table(table, hash);
  hash_entry table_entry = table.data[index];

  size_t total_checked = 0;
  while (table_entry.occupied && table_entry.hash != hash &&
         total_checked < table.count) {
    table_entry = table.data[index++];

    if (index >= table.count) {
      index = 0;
    }

    total_checked++;
  }

  if (total_checked >= table.count) {
    return false;
  }

  if (table_entry.occupied) {
    *value = table_entry.nonce;
    return true;
  }

  return false;
}

void merge_hash_tables(hash_table dest, hash_table src) {
  for (size_t entry_index = 0; entry_index < src.count; ++entry_index) {
    hash_entry source_entry = src.data[entry_index];
    if (source_entry.occupied)
      insert_into_table(dest, source_entry.hash, source_entry.nonce);
  }
}

void debug_table(hash_table table) {
  for (int i = 0; i < table.count; ++i) {
    hash_entry entry = table.data[i];
    if (entry.occupied) {
      printf("[ %zu : %zu ]\n", entry.hash, entry.nonce);
    } else {
      printf("[ <empty> }\n");
    }
  }
}

size_t index_in_table(hash_table table, uint64_t hash) {
  return (size_t)(hash % table.count);
}
