#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "file_actions.h"
#include "hash_table.h"
#include "pdf_actions.h"
#include "toy_hash.h"

unsigned char file_buffer_one[MAX_FILE_SIZE];
unsigned char file_buffer_two[MAX_FILE_SIZE];

void print_help(const char *exe_name);
size_t load_pdf(const char *path, unsigned char *buffer);
void nonce_to_ascii(uint64_t nonce, char *buffer, size_t len);

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

  hash_table table = make_hash_table(MAX_CANDIDATE);

  printf("Building table.\n");
  char nonce_buffer[16];
  for (uint64_t nonce = 0; nonce < MAX_CANDIDATE; nonce++) {
    nonce_to_ascii(nonce, nonce_buffer, 16);
    set_nonce(file_buffer_one, nonce_buffer);
    uint64_t hash = toy_hash(file_buffer_one, file_size_one);
    insert_into_table(table, hash, nonce);

    if (nonce % 100 == 0) {
      printf("... still building table (%d) ...\n", (int)(nonce / 100));
    }
  }

  printf("Table done! Seaching for matches.\n");
  for (uint64_t nonce = 0; nonce < MAX_CANDIDATE; nonce++) {
    nonce_to_ascii(nonce, nonce_buffer, 16);
    set_nonce(file_buffer_two, nonce_buffer);
    uint64_t hash = toy_hash(file_buffer_two, file_size_two);

    if (nonce % 100 == 0) {
      printf("... checking against (%d) ...\n", (int)(nonce / 100));
    }

    uint64_t old_nonce;
    if (find_inside_table(table, hash, &old_nonce)) {
      printf("Found a match:\n"
             "  nonce one: %zu\n"
             "  nonce two: %zu\n"
             "  hash:      %zu\n",
             old_nonce, nonce, hash);
      exit(0);
    }
  }

  printf("Failed to find a match.\n");
  exit(0);
}

//
// General CLI utility functions
//

void print_help(const char *exe_name) {
  printf("Usage: %s <path_1> <path_2>\n"
         "  ... where `path_1' and `path_2' are two\n"
         "  different files that you would like to attack.\n",
         exe_name);
}
