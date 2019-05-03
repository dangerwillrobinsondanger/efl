#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#define EFL_UI_CLICKABLE_PROTECTED 1

#include <Efl_Ui.h>
#include "elm_priv.h"

typedef struct {

} Efl_Ui_Clickable_Util_Data;

static void
_on_press_cb(void *data,
             Evas_Object *obj EINA_UNUSED,
             const char *emission EINA_UNUSED,
             const char *source EINA_UNUSED)
{
   efl_ui_clickable_press(data, 1);
}

static void
_on_unpress_cb(void *data,
             Evas_Object *obj EINA_UNUSED,
             const char *emission EINA_UNUSED,
             const char *source EINA_UNUSED)
{
   efl_ui_clickable_unpress(data, 1);
}

static void
_on_mouse_out(void *data,
             Evas_Object *obj EINA_UNUSED,
             const char *emission EINA_UNUSED,
             const char *source EINA_UNUSED)
{
   efl_ui_clickable_abort(data, 1);
}

EOLIAN static void
_efl_ui_clickable_util_link_to_theme(Efl_Canvas_Layout *object, Efl_Ui_Clickable *clickable)
{
   efl_layout_signal_callback_add(object, "efl,action,press", "*", clickable, _on_press_cb, NULL);
   efl_layout_signal_callback_add(object, "efl,action,unpress", "*", clickable, _on_unpress_cb, NULL);
   efl_layout_signal_callback_add(object, "efl,action,mouse_out", "*", clickable, _on_mouse_out, NULL);
}

static void
_press_cb(void *data, const Efl_Event *ev EINA_UNUSED)
{
   efl_ui_clickable_press(data, 1);
}

static void
_unpress_cb(void *data, const Efl_Event *ev EINA_UNUSED)
{
   efl_ui_clickable_unpress(data, 1);
}

static void
_out_cb(void *data, const Efl_Event *ev EINA_UNUSED)
{
   efl_ui_clickable_abort(data, 1);
}

EFL_CALLBACKS_ARRAY_DEFINE(link_to_object_callbacks,
  {EFL_EVENT_POINTER_DOWN, _press_cb},
  {EFL_EVENT_POINTER_UP, _unpress_cb},
  {EFL_EVENT_POINTER_OUT, _out_cb},
)

EOLIAN static void
_efl_ui_clickable_util_link_to_object(Efl_Input_Interface *object, Efl_Ui_Clickable *clickable)
{
   efl_event_callback_array_add(object, link_to_object_callbacks(), clickable);
}


#include "efl_ui_clickable_util.eo.c"
