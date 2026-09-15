#include <stdio.h>

#include "common.h"
#include "file_actions.h"

//
// File reading utility functions
//

long size_of_file(FILE *file);

size_t load_pdf(const char *path, unsigned char *buffer) {
  FILE *file = fopen(path, "rb");
  if (file == NULL)
    return BAD_FILE_OPEN;

  long expected_size = size_of_file(file);
  if (expected_size < 0 || expected_size > MAX_FILE_SIZE)
    goto load_cleanup;

  size_t received_size = fread(buffer, 1, expected_size, file);
  if (received_size != expected_size)
    goto load_cleanup;

  fclose(file);
  return (size_t)expected_size;

load_cleanup:
  fclose(file);
  return BAD_FILE_OPEN;
}

int write_pdf(const char *path, const unsigned char *buffer, size_t count) {
  FILE *file = fopen(path, "wb");
  if (file == NULL)
    return -1;

  size_t written = fwrite(buffer, 1, count, file);
  if (written != count)
    return -1;

  return 0;
}

long size_of_file(FILE *file) {
  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);

  rewind(file);
  return file_size;
}

//
// Other things
//

void nonce_to_ascii(uint64_t nonce, char *buffer, size_t len) {
  static const char hex[] = {
      '0', '1', '2', '3', '4', '5', '6', '7',
      '8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
  };

  for (int i = len - 1; i >= 0; --i) {
    buffer[i] = hex[nonce & 0xf];
    nonce >>= 4;
  }
}
