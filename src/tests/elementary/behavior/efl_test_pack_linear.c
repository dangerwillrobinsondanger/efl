#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif

#include <Efl_Ui.h>
#include "efl_ui_behavior_suite.h"
#include "suite_helpers.h"

EFL_START_TEST(pack_begin1)
{

}
EFL_END_TEST

void
efl_pack_linear_behavior_test(TCase *tc)
{
   tcase_add_test(tc, pack_begin1);
   efl_pack_behavior_test(tc);
}
