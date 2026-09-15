#ifndef FILE_ACTIONS_H
#define FILE_ACTIONS_H

#include <stddef.h>
#include <stdint.h>

//
// File reading utility functions
//

size_t load_pdf(const char *path, unsigned char *buffer);
int write_pdf(const char *path, const unsigned char *buffer, size_t count);

//
// Other things
//

void nonce_to_ascii(uint64_t nonce, char *buffer, size_t len);

#endif /* FILE_ACTIONS_H */
