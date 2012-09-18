#ifdef HAVE_CONFIG_H
# include <config.h>
#endif /* ifdef HAVE_CONFIG_H */

#include <stdlib.h>
#include <stdio.h>

#include <Eina.h>

#include <check.h>

#include <Eeze.h>
#include <Eeze_Sensor.h>
#include <Eeze_Net.h>

START_TEST(eeze_test_init)
{
   int ret;

   ret = eeze_init();
   fail_if(ret != 1);

   ret = eeze_shutdown();
   fail_if(ret != 0);
}
END_TEST

static void
catch_events(const char *device EINA_UNUSED, Eeze_Udev_Event  event EINA_UNUSED,
             void *data EINA_UNUSED, Eeze_Udev_Watch *watch EINA_UNUSED)
{
}

// FIXME split udev tests into pieces here
START_TEST(eeze_test_udev_types)
{
   Eina_List *type;

   eeze_init();

   type = eeze_udev_find_by_type(EEZE_UDEV_TYPE_NONE, NULL);
   type = eeze_udev_find_unlisted_similar(type);
   type = eeze_udev_find_by_type(EEZE_UDEV_TYPE_KEYBOARD, NULL);
   type = eeze_udev_find_unlisted_similar(type);
   type = eeze_udev_find_by_type(EEZE_UDEV_TYPE_MOUSE, NULL);
   type = eeze_udev_find_by_type(EEZE_UDEV_TYPE_TOUCHPAD, NULL);
   type = eeze_udev_find_unlisted_similar(type);
   type = eeze_udev_find_by_type(EEZE_UDEV_TYPE_DRIVE_MOUNTABLE, NULL);
   type = eeze_udev_find_unlisted_similar(type);
   type = eeze_udev_find_by_type(EEZE_UDEV_TYPE_DRIVE_INTERNAL, NULL);
   type = eeze_udev_find_unlisted_similar(type);
   type = eeze_udev_find_by_type(EEZE_UDEV_TYPE_DRIVE_REMOVABLE, NULL);
   type = eeze_udev_find_unlisted_similar(type);
   type = eeze_udev_find_by_type(EEZE_UDEV_TYPE_DRIVE_CDROM, NULL);
   type = eeze_udev_find_unlisted_similar(type);
   type = eeze_udev_find_by_type(EEZE_UDEV_TYPE_POWER_AC, NULL);
   type = eeze_udev_find_unlisted_similar(type);
   type = eeze_udev_find_by_type(EEZE_UDEV_TYPE_POWER_BAT, NULL);
   type = eeze_udev_find_unlisted_similar(type);
   type = eeze_udev_find_by_type(EEZE_UDEV_TYPE_IS_IT_HOT_OR_IS_IT_COLD_SENSOR, NULL);
   type = eeze_udev_find_unlisted_similar(type);
   type = eeze_udev_find_by_type(EEZE_UDEV_TYPE_NET, NULL);
   type = eeze_udev_find_unlisted_similar(type);
   type = eeze_udev_find_by_type(EEZE_UDEV_TYPE_V4L, NULL);
   type = eeze_udev_find_unlisted_similar(type);
   type = eeze_udev_find_by_type(EEZE_UDEV_TYPE_BLUETOOTH, NULL);
   type = eeze_udev_find_unlisted_similar(type);
   type = eeze_udev_find_by_type(EEZE_UDEV_TYPE_JOYSTICK, NULL);
   type = eeze_udev_find_unlisted_similar(type);
   type = eeze_udev_find_by_type(0, NULL);
   type = eeze_udev_find_unlisted_similar(type);

   eeze_shutdown();
}
END_TEST

START_TEST(eeze_test_udev_watch)
{
   Eeze_Udev_Watch *watch;

   ecore_init();
   eeze_init();

   /* watch never gets triggered as this is run without user interaction */
   watch = eeze_udev_watch_add(EEZE_UDEV_TYPE_KEYBOARD, (EEZE_UDEV_EVENT_ADD | EEZE_UDEV_EVENT_REMOVE), catch_events, NULL);
   watch = eeze_udev_watch_add(EEZE_UDEV_TYPE_DRIVE_INTERNAL, (EEZE_UDEV_EVENT_ADD | EEZE_UDEV_EVENT_REMOVE), catch_events, NULL);
   watch = eeze_udev_watch_add(EEZE_UDEV_TYPE_POWER_AC, (EEZE_UDEV_EVENT_ADD | EEZE_UDEV_EVENT_REMOVE), catch_events, NULL);
   watch = eeze_udev_watch_add(EEZE_UDEV_TYPE_NET, (EEZE_UDEV_EVENT_ADD | EEZE_UDEV_EVENT_REMOVE), catch_events, NULL);
   watch = eeze_udev_watch_add(EEZE_UDEV_TYPE_IS_IT_HOT_OR_IS_IT_COLD_SENSOR, (EEZE_UDEV_EVENT_ADD | EEZE_UDEV_EVENT_REMOVE), catch_events, NULL);
   watch = eeze_udev_watch_add(EEZE_UDEV_TYPE_V4L, (EEZE_UDEV_EVENT_ADD | EEZE_UDEV_EVENT_REMOVE), catch_events, NULL);
   watch = eeze_udev_watch_add(EEZE_UDEV_TYPE_BLUETOOTH, (EEZE_UDEV_EVENT_ADD | EEZE_UDEV_EVENT_REMOVE), catch_events, NULL);
   watch = eeze_udev_watch_add(EEZE_UDEV_TYPE_NONE, (EEZE_UDEV_EVENT_ADD | EEZE_UDEV_EVENT_REMOVE), catch_events, NULL);
   eeze_udev_watch_del(watch);

   eeze_shutdown();
   ecore_shutdown();
}
END_TEST

START_TEST(eeze_test_udev_syspath)
{
   Eina_List *type, *l;
   const char *name, *check;

   eeze_init();

   type = eeze_udev_find_by_type(EEZE_UDEV_TYPE_KEYBOARD, NULL);
   type = eeze_udev_find_unlisted_similar(type);
   EINA_LIST_FOREACH(type, l, name)
     {
         check = eeze_udev_syspath_get_devpath(name);
         eeze_udev_find_similar_from_syspath(name);
         eeze_udev_find_similar_from_syspath(NULL);
         eeze_udev_syspath_get_parent(name);
         eeze_udev_syspath_get_parent(NULL);
         eeze_udev_syspath_get_parents(name);
         eeze_udev_syspath_get_parents(NULL);
         eeze_udev_syspath_get_devname(name);
         eeze_udev_syspath_get_devname(NULL);
         eeze_udev_syspath_get_subsystem(name);
         eeze_udev_syspath_get_sysattr(name, "manufacturer");
         eeze_udev_syspath_is_mouse(name);
         eeze_udev_syspath_is_kbd(name);
         eeze_udev_syspath_is_touchpad(name);
         eeze_udev_syspath_is_joystick(name);
         eeze_udev_walk_get_sysattr(name, "manufacturer");
         eeze_udev_find_by_sysattr("manufacturer", NULL);
         eeze_udev_find_by_sysattr(NULL, NULL);
         eeze_udev_walk_check_sysattr(name, "manufacturer", NULL);
         eeze_udev_walk_check_sysattr(name, "manufacturer", "foo");
         eeze_udev_walk_get_sysattr(NULL, "manufacturer");
         eeze_udev_walk_get_sysattr(name, NULL);
     }

   type = eeze_udev_find_by_type(EEZE_UDEV_TYPE_MOUSE, NULL);
   type = eeze_udev_find_unlisted_similar(type);
   EINA_LIST_FOREACH(type, l, name)
     {
         eeze_udev_syspath_is_mouse(name);
         eeze_udev_syspath_is_touchpad(name);
         eeze_udev_syspath_is_joystick(name);
     }

   type = eeze_udev_find_by_type(EEZE_UDEV_TYPE_TOUCHPAD, NULL);
   type = eeze_udev_find_unlisted_similar(type);
   EINA_LIST_FOREACH(type, l, name)
     {
         eeze_udev_syspath_is_touchpad(name);
     }

   type = eeze_udev_find_by_type(EEZE_UDEV_TYPE_JOYSTICK, NULL);
   type = eeze_udev_find_unlisted_similar(type);
   EINA_LIST_FOREACH(type, l, name)
     {
         eeze_udev_syspath_is_joystick(name);
     }

   eeze_udev_devpath_get_syspath("/dev/null");

   eeze_shutdown();
}
END_TEST

START_TEST(eeze_test_udev_attr)
{
   Eina_List *type;
   const char *name, *check, *check2;

   eeze_init();

   type = eeze_udev_find_by_type(EEZE_UDEV_TYPE_DRIVE_INTERNAL, NULL);
   type = eeze_udev_find_unlisted_similar(type);
   eeze_udev_find_by_filter("backlight", NULL, NULL);

   eeze_udev_find_by_filter(NULL, NULL, NULL);

   type = eeze_udev_find_by_type(EEZE_UDEV_TYPE_NET, NULL);
   type = eeze_udev_find_unlisted_similar(type);
   EINA_LIST_FREE(type, name)
     {
        check = eeze_udev_syspath_get_property(name, "INTERFACE");
     }

   type = eeze_udev_find_by_type(EEZE_UDEV_TYPE_DRIVE_INTERNAL, NULL);
   type = eeze_udev_find_unlisted_similar(type);
   EINA_LIST_FREE(type, name)
     {
        check = eeze_udev_syspath_get_property(name, "ID_SERIAL");
     }

   type = eeze_udev_find_by_type(EEZE_UDEV_TYPE_DRIVE_REMOVABLE, NULL);
   type = eeze_udev_find_unlisted_similar(type);
   EINA_LIST_FREE(type, name)
     {
        if ((check = eeze_udev_syspath_get_sysattr(name, "model")))
          {
             check2 = eeze_udev_syspath_get_subsystem(name);
          }
     }

   type = eeze_udev_find_by_type(EEZE_UDEV_TYPE_V4L, NULL);
   type = eeze_udev_find_unlisted_similar(type);
   EINA_LIST_FREE(type, name)
     {
       if ((check = eeze_udev_syspath_get_property(name, "DEVNAME")))
	 {
	   if ((check2 = eeze_udev_syspath_get_sysattr(name, "name")))
	     {
	       eina_stringshare_del(check2);
	     }
	   eina_stringshare_del(check);
	 }
       eina_stringshare_del(name);
     }

   eeze_shutdown();
}
END_TEST

START_TEST(eeze_test_net_list)
{
   int ret;
   Eina_List *list = NULL;

   ret = eeze_init();
   fail_if(ret != 1);

   list = eeze_net_list();
   fail_if(list == NULL);

   ret = eeze_shutdown();
   fail_if(ret != 0);
}
END_TEST

START_TEST(eeze_test_net_attr)
{
   int ret;
   int idx = 0;
   Eeze_Net *net = NULL;
   const char *tmp = NULL;

   ret = eeze_init();
   fail_if(ret != 1);

   net = eeze_net_new("lo");
   fail_if(net == NULL);

   tmp = eeze_net_mac_get(net);
   fail_if(tmp == NULL);
   tmp = NULL;

   idx = eeze_net_idx_get(net);
   fail_if(!idx);

   eeze_net_scan(net);

   tmp = eeze_net_addr_get(net, EEZE_NET_ADDR_TYPE_IP);
   fail_if(tmp == NULL);
   tmp = NULL;

   tmp = eeze_net_addr_get(net, EEZE_NET_ADDR_TYPE_BROADCAST);
   fail_if(tmp == NULL);
   tmp = NULL;

   tmp = eeze_net_addr_get(net, EEZE_NET_ADDR_TYPE_NETMASK);
   fail_if(tmp == NULL);
   tmp = NULL;

   tmp = eeze_net_attribute_get(net, "carrier");
   fail_if(tmp == NULL);
   tmp = NULL;

   tmp = eeze_net_syspath_get(net);
   fail_if(tmp == NULL);

   eeze_net_free(net);

   ret = eeze_shutdown();
   fail_if(ret != 0);
}
END_TEST

#ifdef HAVE_IPV6
START_TEST(eeze_test_net_attr_ipv6)
{
   int ret;
   Eeze_Net *net = NULL;
   const char *tmp = NULL;

   ret = eeze_init();
   fail_if(ret != 1);

   net = eeze_net_new("lo");
   fail_if(net == NULL);

   tmp = eeze_net_addr_get(net, EEZE_NET_ADDR_TYPE_IP6);
   fail_if(tmp == NULL);
   tmp = NULL;

   tmp = eeze_net_addr_get(net, EEZE_NET_ADDR_TYPE_BROADCAST6);
   fail_if(tmp == NULL);
   tmp = NULL;

   tmp = eeze_net_addr_get(net, EEZE_NET_ADDR_TYPE_NETMASK6);
   fail_if(tmp == NULL);

   eeze_net_free(net);

   ret = eeze_shutdown();
   fail_if(ret != 0);
}
END_TEST
#endif

START_TEST(eeze_test_sensor_handle_get)
{
   Eeze_Sensor *handle;
   int ret;

   ret = eeze_init();
   fail_if(ret != 1);

   handle = eeze_sensor_handle_get();
   fail_if(handle == NULL);

   ret = eeze_shutdown();
   fail_if(ret != 0);
}
END_TEST

START_TEST(eeze_test_sensor_read)
{
   Eeze_Sensor_Obj *sens = NULL;
   int ret = 0;
   float x, y, z;
   int acc;
   unsigned long long timestamp;
   Eina_Bool rc = EINA_FALSE;

   ecore_init();
   ret = eeze_init();
   fail_if(ret != 1);

   sens = eeze_sensor_new(EEZE_SENSOR_TYPE_ACCELEROMETER);
   fail_if(sens == NULL);
   rc = eeze_sensor_read(sens);
   fail_if(rc == EINA_FALSE);
   rc = eeze_sensor_accuracy_get(sens, &acc);
   fail_if(rc == EINA_FALSE);
   rc = eeze_sensor_timestamp_get(sens, &timestamp);
   fail_if(rc == EINA_FALSE);

   sens = eeze_sensor_new(EEZE_SENSOR_TYPE_MAGNETIC);
   fail_if(sens == NULL);
   rc = eeze_sensor_read(sens);
   fail_if(rc == EINA_FALSE);
   rc = eeze_sensor_accuracy_get(sens, &acc);
   fail_if(rc == EINA_FALSE);
   rc = eeze_sensor_timestamp_get(sens, &timestamp);
   fail_if(rc == EINA_FALSE);

   sens = eeze_sensor_new(EEZE_SENSOR_TYPE_ORIENTATION);
   fail_if(sens == NULL);
   rc = eeze_sensor_read(sens);
   fail_if(rc == EINA_FALSE);
   rc = eeze_sensor_accuracy_get(sens, &acc);
   fail_if(rc == EINA_FALSE);
   rc = eeze_sensor_timestamp_get(sens, &timestamp);
   fail_if(rc == EINA_FALSE);

   sens = eeze_sensor_new(EEZE_SENSOR_TYPE_GYROSCOPE);
   fail_if(sens == NULL);
   rc = eeze_sensor_read(sens);
   fail_if(rc == EINA_FALSE);
   rc = eeze_sensor_accuracy_get(sens, &acc);
   fail_if(rc == EINA_FALSE);
   rc = eeze_sensor_timestamp_get(sens, &timestamp);
   fail_if(rc == EINA_FALSE);

   sens = eeze_sensor_new(EEZE_SENSOR_TYPE_PROXIMITY);
   fail_if(sens == NULL);
   rc = eeze_sensor_read(sens);
   fail_if(rc == EINA_FALSE);
   rc = eeze_sensor_accuracy_get(sens, &acc);
   fail_if(rc == EINA_FALSE);
   rc = eeze_sensor_timestamp_get(sens, &timestamp);
   fail_if(rc == EINA_FALSE);

   sens = eeze_sensor_new(EEZE_SENSOR_TYPE_LIGHT);
   fail_if(sens == NULL);
   rc = eeze_sensor_read(sens);
   fail_if(rc == EINA_FALSE);
   rc = eeze_sensor_accuracy_get(sens, &acc);
   fail_if(rc == EINA_FALSE);
   rc = eeze_sensor_timestamp_get(sens, &timestamp);
   fail_if(rc == EINA_FALSE);

   eeze_sensor_free(sens);

   ret = eeze_shutdown();
   fail_if(ret != 0);
}
END_TEST

static Eina_Bool
event_cb(void *data EINA_UNUSED, int ev_type, void *event)
{
   Eeze_Sensor_Obj *sens = NULL;
   int acc;
   unsigned long long timestamp;
   Eina_Bool rc = EINA_FALSE;

   sens = event;
   fail_if(sens == NULL);
   rc = eeze_sensor_accuracy_get(sens, &acc);
   fail_if(rc == EINA_FALSE);
   rc = eeze_sensor_timestamp_get(sens, &timestamp);
   fail_if(rc == EINA_FALSE);

   return ECORE_CALLBACK_PASS_ON;
}

START_TEST(eeze_test_sensor_async_read)
{
   Eeze_Sensor_Obj *sens = NULL;
   int ret = 0;
   Eina_Bool rc = EINA_FALSE;

   ecore_init();
   ret = eeze_init();
   fail_if(ret != 1);

   ecore_event_handler_add(EEZE_SENSOR_EVENT_ACCELEROMETER, event_cb, NULL);
   sens = eeze_sensor_new(EEZE_SENSOR_TYPE_ACCELEROMETER);
   fail_if(sens == NULL);
   rc = eeze_sensor_async_read(sens, NULL);
   fail_if(rc == EINA_FALSE);

   ecore_event_handler_add(EEZE_SENSOR_EVENT_MAGNETIC, event_cb, NULL);
   sens = eeze_sensor_new(EEZE_SENSOR_TYPE_MAGNETIC);
   fail_if(sens == NULL);
   rc = eeze_sensor_async_read(sens, NULL);
   fail_if(rc == EINA_FALSE);

   ecore_event_handler_add(EEZE_SENSOR_EVENT_ORIENTATION, event_cb, NULL);
   sens = eeze_sensor_new(EEZE_SENSOR_TYPE_ORIENTATION);
   fail_if(sens == NULL);
   rc = eeze_sensor_async_read(sens, NULL);
   fail_if(rc == EINA_FALSE);

   ecore_event_handler_add(EEZE_SENSOR_EVENT_GYROSCOPE, event_cb, NULL);
   sens = eeze_sensor_new(EEZE_SENSOR_TYPE_GYROSCOPE);
   fail_if(sens == NULL);
   rc = eeze_sensor_async_read(sens, NULL);
   fail_if(rc == EINA_FALSE);

   ecore_event_handler_add(EEZE_SENSOR_EVENT_PROXIMITY, event_cb, NULL);
   sens = eeze_sensor_new(EEZE_SENSOR_TYPE_PROXIMITY);
   fail_if(sens == NULL);
   rc = eeze_sensor_async_read(sens, NULL);
   fail_if(rc == EINA_FALSE);

   ecore_event_handler_add(EEZE_SENSOR_EVENT_LIGHT, event_cb, NULL);
   sens = eeze_sensor_new(EEZE_SENSOR_TYPE_LIGHT);
   fail_if(sens == NULL);
   rc = eeze_sensor_async_read(sens, NULL);
   fail_if(rc == EINA_FALSE);

   eeze_sensor_free(sens);

   ret = eeze_shutdown();
   fail_if(ret != 0);
}
END_TEST

START_TEST(eeze_test_sensor_obj_get)
{
   Eeze_Sensor_Obj *obj = NULL, *obj_tmp = NULL;
   int ret = 0;

   ecore_init();
   ret = eeze_init();
   fail_if(ret != 1);

   obj = calloc(1, sizeof(Eeze_Sensor_Obj));
   fail_if(obj == NULL);

   obj_tmp = obj;

   obj = eeze_sensor_obj_get(EEZE_SENSOR_TYPE_ACCELEROMETER);
   fail_if(obj == obj_tmp);

   free(obj);

   ret = eeze_shutdown();
   fail_if(ret != 0);
}
END_TEST

Suite *
eeze_suite(void)
{
   Suite *s;
   TCase *tc;

   s = suite_create("Eeze");

   tc = tcase_create("Eeze_Init");
   tcase_add_test(tc, eeze_test_init);
   suite_add_tcase(s, tc);

   tc = tcase_create("Eeze_Udev");
   tcase_add_test(tc, eeze_test_udev_types);
   tcase_add_test(tc, eeze_test_udev_watch);
   tcase_add_test(tc, eeze_test_udev_syspath);
   tcase_add_test(tc, eeze_test_udev_attr);
   suite_add_tcase(s, tc);

   tc = tcase_create("Eeze_Net");
   tcase_add_test(tc, eeze_test_net_list);
   tcase_add_test(tc, eeze_test_net_attr);
#ifdef HAVE_IPV6
   //FIXME Figure out why we fail for the ipv6 tests here (code or test)
   //tcase_add_test(tc, eeze_test_net_attr_ipv6);
#endif
   suite_add_tcase(s, tc);

   tc = tcase_create("Eeze Sensor");
   tcase_add_test(tc, eeze_test_sensor_handle_get);
   tcase_add_test(tc, eeze_test_sensor_read);
   tcase_add_test(tc, eeze_test_sensor_async_read);
   tcase_add_test(tc, eeze_test_sensor_obj_get);
   suite_add_tcase(s, tc);

   return s;
}

int
main(void)
{
   Suite *s;
   SRunner *sr;
   int failed_count;

   s = eeze_suite();
   sr = srunner_create(s);
   srunner_run_all(sr, CK_ENV);
   failed_count = srunner_ntests_failed(sr);
   srunner_free(sr);

   return (failed_count == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

