#ifndef EFL_UI_WIDGET_COMMON_H
#define EFL_UI_WIDGET_COMMON_H

/**
 *
 */
EAPI Eina_Iterator* efl_ui_widget_tree_iterator(Efl_Ui_Widget *obj);

/**
 *
 */
EAPI Eina_Iterator* efl_ui_widget_tree_widget_iterator(Efl_Ui_Widget *obj);

/**
 *
 */
EAPI Eina_Iterator* efl_ui_widget_parent_iterator(Efl_Ui_Widget *obj);

#endif
