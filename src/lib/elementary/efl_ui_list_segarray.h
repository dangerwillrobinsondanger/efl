#ifndef EFL_UI_LIST_SEGARRAY_H
#define EFL_UI_LIST_SEGARRAY_H

#include "efl_ui_list_segarray.h"

typedef struct _Efl_Ui_List_SegArray_Node
{
   EINA_RBTREE;
  
   int length;
   int max;
   int first;

   // Eina_Position2D initial_position;

   Efl_Ui_List_Item* pointers[0];
} Efl_Ui_List_SegArray_Node;

typedef struct _Efl_Ui_List_SegArray
{
   Eina_Rbtree *root; // of Efl_Ui_List_SegArray_Nodea

   int array_initial_size;
} Efl_Ui_List_SegArray;

Eina_Accessor* efl_ui_list_segarray_accessor_get(Efl_Ui_List_SegArray* segarray);
void efl_ui_list_segarray_insert_accessor(int first, Eina_Accessor* accessor);

#endif
