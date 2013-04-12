#ifndef _ECORE_SUITE_H
#define _ECORE_SUITE_H

#include <check.h>

void ecore_test_ecore(TCase *tc);
void ecore_test_ecore_con(TCase *tc);
void ecore_test_ecore_x(TCase *tc);
void ecore_test_ecore_imf(TCase *tc);
void ecore_test_ecore_audio(TCase *tc);
void ecore_test_coroutine(TCase *tc);
void ecore_test_timer(TCase *tc);

#endif /* _ECORE_SUITE_H */
