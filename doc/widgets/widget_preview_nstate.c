#include "widget_preview_tmpl_head.c"

Evas_Object *o = efl_add(EFL_UI_NSTATE_CLASS, win);
evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
elm_win_resize_object_add(win, o);
evas_object_show(o);

elm_object_text_set(o, "Nstate");

#include "widget_preview_tmpl_foot.c"
