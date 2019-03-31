#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>

#include <Eina.h>
#include <Eio.h>
#include <Ecore.h>

static Eina_Value
_process_children_cb(Eo *o EINA_UNUSED, void *data EINA_UNUSED, const Eina_Value v)
{
   Efl_Model *child = NULL;
   unsigned int i, len;

 if (eina_value_type_get(&v) == EINA_VALUE_TYPE_ERROR)
     return v;

   EINA_VALUE_ARRAY_FOREACH(&v, len, i, child)
     {
        const char *child_path = "";
        Eina_Value *v = efl_model_property_get(child, "path");
        eina_value_string_convert(v, &child_path);
        printf("PATH: %s\n", child_path);
     }

   return v;
}

static void
_children(void *data, const Efl_Event *ev)
{
   Efl_Io_Model *m = data;
   Eina_Future *future;

   printf("%d\n", efl_model_children_count_get(m));
   future = efl_model_children_slice_get(m, 0, efl_model_children_count_get(m));
   future = efl_future_then(m, future, .success_type = EINA_VALUE_TYPE_ARRAY,
                          .success = _process_children_cb,
                          .free = NULL,
                          .data = NULL);
}

static void
_properties_changed(void *data, const Efl_Event *ev)
{
   efl_model_children_count_get(data);
}


static void
_children_removed_cb(void *d EINA_UNUSED, const Efl_Event* event)
{
   Efl_Model_Children_Event* evt = event->info;
   Eina_Future *future;

   printf("GO GO GO!\n");
}

static void
_children_added_cb(void *d EINA_UNUSED, const Efl_Event* event)
{
   Efl_Model_Children_Event* evt = event->info;
   Eina_Future *future;

   printf("GO GO GO!\n");
}

void list_files(void *data)
{

   Efl_Io_Model *m = efl_add(EFL_IO_MODEL_CLASS, efl_app_main_get(),
                        efl_io_model_path_set(efl_added, getenv("HOME")),
                        efl_loop_model_volatile_make(efl_added)
                      );
   efl_ref(m);
   //efl_event_callback_add(m, EFL_MODEL_EVENT_PROPERTIES_CHANGED, _properties_changed, m);
   //efl_event_callback_add(m, EFL_MODEL_EVENT_CHILDREN_COUNT_CHANGED, _children, m);
   efl_event_callback_add(m, EFL_MODEL_EVENT_CHILD_ADDED, &_children_added_cb, m);
   efl_event_callback_add(m, EFL_MODEL_EVENT_CHILD_REMOVED, &_children_removed_cb, NULL);

}

int main(int argc, char const *argv[])
{
   const char *path;

   eio_init();
   ecore_init();

   path = getenv("HOME");

   if (argc > 1)
     path = argv[1];

   ecore_job_add(&list_files, path);

   ecore_main_loop_begin();

   ecore_shutdown();
   eio_shutdown();
   return 0;
}
