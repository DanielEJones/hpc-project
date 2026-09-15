#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint64_t hash;
  uint64_t nonce;
  bool occupied;
} hash_entry;

typedef struct {
  hash_entry *data;
  size_t count;
} hash_table;

hash_table make_hash_table(size_t entry_count);
void free_hash_table(hash_table *table);

bool insert_into_table(hash_table table, uint64_t hash, uint64_t value);
bool find_inside_table(hash_table table, uint64_t hash, uint64_t *value);
void merge_hash_tables(hash_table dest, hash_table src);

void debug_table(hash_table table);

#endif /* HASH_TABLE_H */
