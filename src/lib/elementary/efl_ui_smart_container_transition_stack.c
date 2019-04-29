#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif

#define EFL_PACK_LAYOUT_PROTECTED

#include <Elementary.h>
#include "elm_priv.h"


typedef struct {
   Efl_Ui_Smart_Container * container;
   Efl_Gfx_Entity *group;
   Efl_Canvas_Animation_Player *hide, *show;
   int from, to;
   Efl_Gfx_Entity *content;
   Eina_Size2D page_size;
} Efl_Ui_Smart_Container_Transition_Stack_Data;

#define MY_CLASS EFL_UI_SMART_CONTAINER_TRANSITION_STACK_CLASS

static void
_geom_sync(Eo *obj EINA_UNUSED, Efl_Ui_Smart_Container_Transition_Stack_Data *pd)
{
   Eina_Array *array = eina_array_new(2);
   Eina_Rect group_pos = efl_gfx_entity_geometry_get(pd->group);
   if (efl_player_play_get(pd->hide))
     {
        //we are currently in animation, sync the geometry of the targets
        eina_array_push(array, efl_animation_player_target_get(pd->hide));
        eina_array_push(array, efl_animation_player_target_get(pd->show));
     }
   else
     {
        //we only have our content right now, or nothing
        eina_array_push(array, pd->content);
     }
   Eina_Rect goal = EINA_RECT_EMPTY();
   goal.size = pd->page_size;
   goal.y = (group_pos.y + group_pos.h/2)-pd->page_size.h/2;
   goal.x = (group_pos.x + group_pos.w/2)-pd->page_size.w/2;
   while (eina_array_count(array) > 0)
     {
        Efl_Gfx_Entity *subobj = eina_array_pop(array);
        efl_gfx_entity_geometry_set(subobj, goal);
     }
   eina_array_free(array);
}

static void
_resize_cb(void *data, const Efl_Event *ev EINA_UNUSED)
{
   _geom_sync(data, efl_data_scope_get(data, MY_CLASS));
}

static void
_move_cb(void *data, const Efl_Event *ev EINA_UNUSED)
{
   _geom_sync(data, efl_data_scope_get(data, MY_CLASS));
}

static void
_running_cb(void *data, const Efl_Event *ev EINA_UNUSED)
{
   Efl_Ui_Smart_Container_Transition_Stack_Data *pd = efl_data_scope_safe_get(data, MY_CLASS);

   //calculate absolut position, multiply pos with 2.0 because duration is only 0.5)
   double absolut_position = pd->from + (pd->to - pd->from)*(efl_player_pos_get(pd->show)*2.0);
   efl_event_callback_call(data, EFL_UI_SMART_CONTAINER_TRANSITION_EVENT_POS_UPDATE, &absolut_position);
}

static void
_anim_started_cb(void *data EINA_UNUSED, const Efl_Event *event)
{
   Efl_Canvas_Object *content;

   content = efl_animation_player_target_get(event->object);
   efl_gfx_entity_visible_set(content, EINA_TRUE);

   efl_event_freeze(content);
}

static void
_hide_anim_ended_cb(void *data, const Efl_Event *event EINA_UNUSED)
{
   Efl_Ui_Smart_Container_Transition_Stack_Data *pd = efl_data_scope_safe_get(data, MY_CLASS);
   Efl_Canvas_Object *content;

   EINA_SAFETY_ON_NULL_RETURN(pd);
   content = efl_animation_player_target_get(pd->hide);
   efl_gfx_entity_visible_set(content, EINA_FALSE);
   efl_event_thaw(content);
}

static void
_show_anim_ended_cb(void *data, const Efl_Event *event EINA_UNUSED)
{
   Efl_Ui_Smart_Container_Transition_Stack_Data *pd = efl_data_scope_safe_get(data, MY_CLASS);
   Efl_Canvas_Object *content;

   EINA_SAFETY_ON_NULL_RETURN(pd);
   content = efl_animation_player_target_get(pd->show);
   efl_gfx_entity_visible_set(content, EINA_TRUE);
   efl_event_thaw(content);
   pd->content = content;
}

EFL_CALLBACKS_ARRAY_DEFINE(_anim_show_event_cb,
  {EFL_ANIMATION_PLAYER_EVENT_RUNNING, _running_cb},
  {EFL_ANIMATION_PLAYER_EVENT_STARTED, _anim_started_cb},
  {EFL_ANIMATION_PLAYER_EVENT_ENDED, _show_anim_ended_cb},
)

EFL_CALLBACKS_ARRAY_DEFINE(_anim_hide_event_cb,
  {EFL_ANIMATION_PLAYER_EVENT_STARTED, _anim_started_cb},
  {EFL_ANIMATION_PLAYER_EVENT_ENDED, _hide_anim_ended_cb},
)

EOLIAN static void
_efl_ui_smart_container_transition_stack_efl_ui_smart_container_transition_smart_container_set(Eo *obj, Efl_Ui_Smart_Container_Transition_Stack_Data *pd, Efl_Ui_Smart_Container *smart_container, Efl_Canvas_Group *group)
{
   if (smart_container && group)
     {
        Efl_Canvas_Animation_Alpha *show_anim, *hide_anim;
        pd->container = smart_container;
        pd->group = group;

        efl_event_callback_add(pd->group, EFL_GFX_ENTITY_EVENT_SIZE_CHANGED, _resize_cb, obj);
        efl_event_callback_add(pd->group, EFL_GFX_ENTITY_EVENT_POSITION_CHANGED, _move_cb, obj);

        show_anim = efl_add(EFL_CANVAS_ANIMATION_ALPHA_CLASS, obj);
        efl_animation_alpha_set(show_anim, 0.0, 1.0);
        efl_animation_duration_set(show_anim, 0.5);
        efl_animation_final_state_keep_set(show_anim, EINA_TRUE);

        pd->show = efl_add(EFL_CANVAS_ANIMATION_PLAYER_CLASS, obj);
        efl_animation_player_animation_set(pd->show, show_anim);
        efl_player_play_set(pd->show, EINA_FALSE);
        efl_event_callback_array_add(pd->show, _anim_show_event_cb(), obj);

        //Default Hide Animation
        hide_anim = efl_add(EFL_CANVAS_ANIMATION_ALPHA_CLASS, obj);
        efl_animation_alpha_set(hide_anim, 1.0, 0.0);
        efl_animation_duration_set(hide_anim, 0.5);
        efl_animation_final_state_keep_set(hide_anim, EINA_TRUE);

        pd->hide = efl_add(EFL_CANVAS_ANIMATION_PLAYER_CLASS, obj);
        efl_animation_player_animation_set(pd->hide, hide_anim);
        efl_player_play_set(pd->hide, EINA_FALSE);
        efl_event_callback_array_add(pd->hide, _anim_hide_event_cb(), obj);

        for (int i = 0; i < efl_content_count(smart_container) ; ++i) {
           Efl_Gfx_Entity *elem = efl_pack_content_get(smart_container, i);
           efl_canvas_group_member_add(pd->group, elem);
           efl_gfx_entity_visible_set(elem, EINA_FALSE);
        }
     }
}

EOLIAN static void
_efl_ui_smart_container_transition_stack_efl_ui_smart_container_transition_content_update(Eo *obj EINA_UNUSED, Efl_Ui_Smart_Container_Transition_Stack_Data *pd, Efl_Gfx_Entity *subobj, int index EINA_UNUSED, Eina_Bool deletion)
{
   if (deletion)
     {
        efl_canvas_group_member_remove(pd->group, subobj);
     }
   else
     {
        efl_canvas_group_member_add(pd->group, subobj);
        efl_gfx_entity_visible_set(subobj, EINA_FALSE);
     }
}

static void
_setup_anim(Efl_Animation_Player *player, Efl_Gfx_Entity *entity)
{
   efl_player_stop(player);
   efl_animation_player_target_set(player, entity);
   efl_player_start(player);
}

EOLIAN static void
_efl_ui_smart_container_transition_stack_efl_ui_smart_container_transition_request_switch(Eo *obj, Efl_Ui_Smart_Container_Transition_Stack_Data *pd, int from, int to)
{
   if (from != -1)
     {
        _setup_anim(pd->hide, efl_pack_content_get(pd->container, from));
        _setup_anim(pd->show, efl_pack_content_get(pd->container, to));
        pd->from = from;
        pd->to = to;
        pd->content = NULL;
     }
   else
     {
        pd->content = efl_pack_content_get(pd->container, to);
        efl_gfx_entity_visible_set(pd->content, EINA_TRUE);
     }

   _geom_sync(obj, pd);
}

EOLIAN static void
_efl_ui_smart_container_transition_stack_efl_ui_smart_container_transition_page_size_set(Eo *obj, Efl_Ui_Smart_Container_Transition_Stack_Data *pd, Eina_Size2D size)
{
   pd->page_size = size;
   _geom_sync(obj, pd);
}


#include "efl_ui_smart_container_transition_stack.eo.c"
