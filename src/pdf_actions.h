#ifndef PDF_ACTIONS_H
#define PDF_ACTIONS_H

#include <stdint.h>

void set_nonce(unsigned char *pdf, const char *nonce);
void set_student_number(unsigned char *pdf, const char *id);

#endif /* PDF_ACTIONS_H */
