#include <string.h>

#include "pdf_actions.h"

#define NONCE_LENGTH 16
#define NONCE_OFFSET 16

#define STUDENT_ID_LENGTH 8
#define STUDENT_ID_OFFSET 45

void set_nonce(unsigned char *pdf, const char *nonce) {
  memcpy(pdf + NONCE_OFFSET, nonce, NONCE_OFFSET);
}

void set_student_number(unsigned char *pdf, const char *id) {
  memcpy(pdf + STUDENT_ID_OFFSET, id, STUDENT_ID_LENGTH);
}
