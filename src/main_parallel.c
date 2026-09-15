#include <omp.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "file_actions.h"
#include "hash_table.h"
#include "pdf_actions.h"
#include "toy_hash.h"

unsigned char file_buffer_one[MAX_FILE_SIZE];
unsigned char file_buffer_two[MAX_FILE_SIZE];

static void print_help(const char *exe_name);
static size_t get_partition(uint64_t hash);

int main(int argc, char **argv) {
  if (argc != 3) {
    fprintf(stderr, "Expected 2 arguments, got %d instead.\n", argc - 1);
    print_help(argv[0]);
    exit(1);
  }

  const char *file_name_one = argv[1];
  const char *file_name_two = argv[2];

  const size_t file_size_one = load_pdf(file_name_one, file_buffer_one);
  if (file_size_one == BAD_FILE_OPEN) {
    fprintf(stderr, "Failed to open file `%s'.\n", file_name_one);
    exit(1);
  }

  const size_t file_size_two = load_pdf(file_name_two, file_buffer_two);
  if (file_size_two == BAD_FILE_OPEN) {
    fprintf(stderr, "Failed to open file `%s'.\n", file_name_two);
    exit(1);
  }

  set_student_number(file_buffer_one, "23821639");
  set_student_number(file_buffer_two, "23821639");

  hash_table tables[NUM_PARTITION];
  omp_lock_t locks[NUM_PARTITION];

  for (int i = 0; i < NUM_PARTITION; ++i) {
    tables[i] = make_hash_table(MAX_CANDIDATE / NUM_PARTITION);
    omp_init_lock(&locks[i]);
  }

  printf("Building Tables.\n");
  double build_tables_starts = omp_get_wtime();

#pragma omp parallel
  {
    unsigned char local_buffer[MAX_FILE_SIZE];
    memcpy(local_buffer, file_buffer_one, file_size_one);
    char nonce_buffer[16];

#pragma omp for
    for (uint64_t nonce = 0; nonce < MAX_CANDIDATE; nonce++) {
      nonce_to_ascii(nonce, nonce_buffer, 16);
      set_nonce(local_buffer, nonce_buffer);

      uint64_t hash = toy_hash(local_buffer, file_size_one);
      size_t partition = get_partition(hash);

      omp_set_lock(&locks[partition]);
      insert_into_table(tables[partition], hash, nonce);
      omp_unset_lock(&locks[partition]);
    }
  }
  double build_tables_ends = omp_get_wtime();
  printf("Table construction complete. (%lf seconds)\n",
         build_tables_ends - build_tables_starts);

  omp_lock_t result_lock;
  omp_init_lock(&result_lock);

  bool found = false;
  uint64_t nonce_one, nonce_two, hash_result;

  printf("Searching for a match.\n");
  double search_starts = omp_get_wtime();
#pragma omp parallel
  {
    unsigned char local_buffer[MAX_FILE_SIZE];
    memcpy(local_buffer, file_buffer_two, file_size_two);
    char nonce_buffer[16];

#pragma omp for
    for (size_t nonce = 0; nonce < MAX_CANDIDATE; nonce++) {
      if (found)
        continue;

      nonce_to_ascii(nonce, nonce_buffer, 16);
      set_nonce(local_buffer, nonce_buffer);

      uint64_t hash = toy_hash(local_buffer, file_size_two);
      size_t partition = get_partition(hash);

      uint64_t old_nonce;
      if (find_inside_table(tables[partition], hash, &old_nonce)) {
        omp_set_lock(&result_lock);
        found = true;
        nonce_one = old_nonce;
        nonce_two = nonce;
        hash_result = hash;
        omp_unset_lock(&result_lock);
      }
    }
  }

  double search_ends = omp_get_wtime();
  printf("Seach ended. (%lf seconds)\n", search_ends - search_starts);

  if (found) {
    char nonce_buffer[16];

    nonce_to_ascii(nonce_one, nonce_buffer, 16);
    set_nonce(file_buffer_one, nonce_buffer);
    write_pdf("/group/cits/cits053/a.out.pdf", file_buffer_one, file_size_one);

    nonce_to_ascii(nonce_two, nonce_buffer, 16);
    set_nonce(file_buffer_two, nonce_buffer);
    write_pdf("/group/cits/cits053/b.out.pdf", file_buffer_two, file_size_two);

    printf("Match found:\n"
           "  nonce one: %zu\n"
           "  nonce two: %zu\n"
           "  hash:      %zu\n",
           nonce_one, nonce_two, hash_result);
  } else {
    printf("Failed to match.\n");
  }

  fflush(stdout);
  fflush(stderr);

  omp_destroy_lock(&result_lock);

  for (int i = 0; i < NUM_PARTITION; ++i) {
    omp_destroy_lock(&locks[i]);
    free_hash_table(&tables[i]);
  }
}

//
// General CLI utility functions
//

static inline void print_help(const char *exe_name) {
  printf("Usage: %s <path_1> <path_2>\n"
         "  ... where `path_1' and `path_2' are two\n"
         "  different files that you would like to attack.\n",
         exe_name);
}

static inline uint64_t get_partition(uint64_t hash) {
  return hash % NUM_PARTITION;
}
