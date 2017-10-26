#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif

#define EFL_UI_POPUP_PROTECTED

#include <Elementary.h>

#include "elm_priv.h"
#include "efl_ui_popup_private.h"

#define MY_CLASS EFL_UI_POPUP_CLASS
#define MY_CLASS_NAME "Efl.Ui.Popup"

static void
_bg_clicked_cb(void *data,
               Eo *o EINA_UNUSED,
               const char *emission EINA_UNUSED,
               const char *source EINA_UNUSED)
{
   Eo *obj = data;
   efl_event_callback_call(obj, EFL_UI_POPUP_EVENT_BG_CLICKED, NULL);
}

EOLIAN static void
_efl_ui_popup_efl_gfx_position_set(Eo *obj, Efl_Ui_Popup_Data *pd EINA_UNUSED, Eina_Position2D pos)
{
   pd->align = EFL_UI_POPUP_ALIGN_NONE;
   efl_gfx_position_set(efl_super(obj, MY_CLASS), pos);
}

static void
_calc_align(Eo *obj)
{
   Efl_Ui_Popup_Data *pd = efl_data_scope_get(obj, MY_CLASS);

   Eina_Rect p_geom = efl_gfx_geometry_get(pd->win_parent);

   efl_gfx_position_set(pd->event_bg, EINA_POSITION2D(0, 0));
   efl_gfx_size_set(pd->event_bg, EINA_SIZE2D(p_geom.w, p_geom.h));

   Eina_Rect o_geom = efl_gfx_geometry_get(obj);

   Evas_Coord pw, ph;
   pw = p_geom.w;
   ph = p_geom.h;

   Evas_Coord ow, oh;
   ow = o_geom.w;
   oh = o_geom.h;

   switch (pd->align)
     {
      case EFL_UI_POPUP_ALIGN_CENTER:
         efl_gfx_position_set(efl_super(obj, MY_CLASS), EINA_POSITION2D((pw - ow ) / 2, (ph - oh) / 2));
         break;
      case EFL_UI_POPUP_ALIGN_LEFT:
         efl_gfx_position_set(efl_super(obj, MY_CLASS), EINA_POSITION2D(0, (ph - oh) / 2));
         break;
      case EFL_UI_POPUP_ALIGN_RIGHT:
         efl_gfx_position_set(efl_super(obj, MY_CLASS), EINA_POSITION2D(pw - ow, (ph - oh) / 2));
         break;
      case EFL_UI_POPUP_ALIGN_TOP:
         efl_gfx_position_set(efl_super(obj, MY_CLASS), EINA_POSITION2D((pw - ow) / 2, 0));
         break;
      case EFL_UI_POPUP_ALIGN_BOTTOM:
         efl_gfx_position_set(efl_super(obj, MY_CLASS), EINA_POSITION2D((pw - ow) / 2, ph - oh));
         break;
      default:
         break;
     }
}

static void
_parent_geom_cb(void *data, const Efl_Event *ev EINA_UNUSED)
{
   _calc_align(data);
}

EOLIAN static void
_efl_ui_popup_elm_widget_widget_parent_set(Eo *obj, Efl_Ui_Popup_Data *pd EINA_UNUSED, Eo *parent EINA_UNUSED)
{
   pd->win_parent = efl_provider_find(obj, EFL_UI_WIN_CLASS);
   if (!pd->win_parent)
     {
        ERR("Cannot find window parent");
        return;
     }

   Eina_Rect p_geom = efl_gfx_geometry_get(pd->win_parent);

   efl_gfx_position_set(pd->event_bg, EINA_POSITION2D(p_geom.x, p_geom.y));
   efl_gfx_size_set(pd->event_bg, EINA_SIZE2D(p_geom.w, p_geom.h));

   efl_event_callback_add(pd->win_parent, EFL_GFX_EVENT_RESIZE, _parent_geom_cb, obj);
   efl_event_callback_add(pd->win_parent, EFL_GFX_EVENT_MOVE, _parent_geom_cb, obj);
}

EOLIAN static Eina_Bool
_efl_ui_popup_efl_container_content_set(Eo *obj, Efl_Ui_Popup_Data *pd EINA_UNUSED, Eo *content)
{
   return efl_content_set(efl_part(obj, "elm.swallow.content"), content);
}

EOLIAN static Eo*
_efl_ui_popup_efl_container_content_get(Eo *obj, Efl_Ui_Popup_Data *pd EINA_UNUSED)
{
   return efl_content_get(efl_part(obj, "elm.swallow.content"));
}

EOLIAN static Eo*
_efl_ui_popup_efl_container_content_unset(Eo *obj, Efl_Ui_Popup_Data *pd EINA_UNUSED)
{
   return efl_content_unset(efl_part(obj, "elm.swallow.content"));
}

EOLIAN static void
_efl_ui_popup_align_set(Eo *obj EINA_UNUSED, Efl_Ui_Popup_Data *pd, Efl_Ui_Popup_Align type)
{
   pd->align = type;
   _calc_align(obj);
}

EOLIAN static Efl_Ui_Popup_Align
_efl_ui_popup_align_get(Eo *obj EINA_UNUSED, Efl_Ui_Popup_Data *pd)
{
   return pd->align;
}

static Eina_Bool
_timer_cb(void *data)
{
   Eo *popup = data;
   efl_event_callback_call(popup, EFL_UI_POPUP_EVENT_TIMEOUT, NULL);
   efl_del(popup);

   return ECORE_CALLBACK_CANCEL;
}

static void
_timer_del(Efl_Ui_Popup_Data *pd)
{
   if (pd->timer)
     {
        ecore_timer_del(pd->timer);
        pd->timer = NULL;
     }
}

static void
_timer_init(Eo *obj, Efl_Ui_Popup_Data *pd)
{
   if (pd->timeout > 0.0)
     pd->timer = ecore_timer_add(pd->timeout, _timer_cb, obj);
}

EOLIAN static void
_efl_ui_popup_efl_gfx_visible_set(Eo *obj, Efl_Ui_Popup_Data *pd, Eina_Bool v)
{
   if (_evas_object_intercept_call(obj, EVAS_OBJECT_INTERCEPT_CB_VISIBLE, 0, v))
     return;

   efl_gfx_visible_set(efl_super(obj, MY_CLASS), v);

   if (v)
     {
        _timer_del(pd);
        _timer_init(obj, pd);
     }
}

EOLIAN static void
_efl_ui_popup_timeout_set(Eo *obj, Efl_Ui_Popup_Data *pd, double time)
{
   if (time < 0.0)
     time = 0.0;

   pd->timeout = time;

   _timer_del(pd);

   if (efl_gfx_visible_get(obj))
     _timer_init(obj, pd);
}

EOLIAN static double
_efl_ui_popup_timeout_get(Eo *obj EINA_UNUSED, Efl_Ui_Popup_Data *pd)
{
   return pd->timeout;
}

EOLIAN static Eo *
_efl_ui_popup_efl_object_constructor(Eo *obj, Efl_Ui_Popup_Data *pd)
{
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd, NULL);

   obj = efl_constructor(efl_super(obj, MY_CLASS));
   efl_canvas_object_type_set(obj, MY_CLASS_NAME);

   elm_widget_sub_object_parent_add(obj);

   elm_widget_can_focus_set(obj, EINA_TRUE);
   elm_layout_theme_set(obj, "popup", "base", "view");

   pd->event_bg = edje_object_add(evas_object_evas_get(obj));
   elm_widget_theme_object_set(obj, pd->event_bg, "popup", "base", "event_bg");
   evas_object_smart_member_add(pd->event_bg, obj);
   evas_object_stack_below(pd->event_bg, wd->resize_obj);

   edje_object_signal_callback_add(pd->event_bg, "elm,action,clicked", "*",
                                   _bg_clicked_cb, obj);

   pd->align = EFL_UI_POPUP_ALIGN_CENTER;

   return obj;
}

EOLIAN static void
_efl_ui_popup_efl_object_destructor(Eo *obj, Efl_Ui_Popup_Data *pd)
{
   ELM_SAFE_DEL(pd->event_bg);

   efl_event_callback_del(pd->win_parent, EFL_GFX_EVENT_RESIZE, _parent_geom_cb,
                          obj);
   efl_event_callback_del(pd->win_parent, EFL_GFX_EVENT_MOVE, _parent_geom_cb,
                          obj);

   efl_destructor(efl_super(obj, MY_CLASS));
}

EOLIAN static void
_efl_ui_popup_elm_layout_sizing_eval(Eo *obj, Efl_Ui_Popup_Data *pd EINA_UNUSED)
{
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd);
   Evas_Coord minw = -1, minh = -1;

   elm_coords_finger_size_adjust(1, &minw, 1, &minh);
   edje_object_size_min_restricted_calc
      (wd->resize_obj, &minw, &minh, minw, minh);
   efl_gfx_size_hint_min_set(obj, EINA_SIZE2D(minw, minh));

   Eina_Size2D size = efl_gfx_size_get(obj);

   Eina_Size2D new_size;
   new_size.w = (minw > size.w ? minw : size.w);
   new_size.h = (minh > size.h ? minh : size.h);
   efl_gfx_size_set(obj, new_size);

   _calc_align(obj);
}

EOLIAN static void
_efl_ui_popup_bg_set(Eo *obj, Efl_Ui_Popup_Data *pd, const char* file, const char* group)
{
   Eo *prev_obj = edje_object_part_swallow_get(pd->event_bg, "elm.swallow.image");
   if (prev_obj)
     {
        edje_object_part_unswallow(pd->event_bg, prev_obj);
        efl_del(prev_obj);
     }

   Eo *image = elm_image_add(obj);
   Eina_Bool ret = elm_image_file_set(image, file, group);
   if (!ret)
     {
        edje_object_signal_emit(pd->event_bg, "elm,state,image,hidden", "elm");
        efl_del(image);
        return;
     }
   edje_object_part_swallow(pd->event_bg, "elm.swallow.image", image);
   edje_object_signal_emit(pd->event_bg, "elm,state,image,visible", "elm");
}

EOLIAN static void
_efl_ui_popup_bg_repeat_events_set(Eo *obj EINA_UNUSED, Efl_Ui_Popup_Data *pd, Eina_Bool repeat)
{
   pd->bg_repeat_events = repeat;

   efl_canvas_object_repeat_events_set(pd->event_bg, repeat);
}

EOLIAN static Eina_Bool
_efl_ui_popup_bg_repeat_events_get(Eo *obj EINA_UNUSED, Efl_Ui_Popup_Data *pd)
{
   return pd->bg_repeat_events;
}

/* Internal EO APIs and hidden overrides */

#define EFL_UI_POPUP_EXTRA_OPS \
   ELM_LAYOUT_SIZING_EVAL_OPS(efl_ui_popup)

#include "efl_ui_popup.eo.c"
