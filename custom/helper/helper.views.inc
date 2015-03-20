<?php

/**
 * @file
 * Drupal Module: helper
 * Adds custom code specific to views.
 */

/**
 * Implements hook_views_query_alter().
 */
function helper_views_query_alter(&$view, &$query) {
  global $language;
  
  // Alter Courses block to display the product by current language.
  // Since the Commerce Product entity does not manage well the language settings, we use
  // instead a custom language field and modify the query condition related to the language here.
  // See Views: Courses, block_1, condition related to the language.
  if ( $view -> name == 'courses' && $view -> current_display == 'block_1' ) {
    //dsm($view, 'before');
	//dsm($query, 'before');
	$query -> where[1]['conditions'][4]['value'] = $language -> language;
    //dsm($query, 'after');
  }
}

/**
 * Implements hook_views_pre_build().
 */
function helper_views_pre_build(&$view) {
  // Remove fields by permission.
  if ( $view->name == 'section_events' ) {
    // Remove Edit, Delete, Add fields
    $fields_to_remove = array('nothing','nothing_1','nothing_2');

    if(!user_access('create product_display content')) {
      foreach ($fields_to_remove as $field_name) {
        //unset($view->field[$field_name]);
        unset($view->display_handler->handlers['field'][$field_name]);
      }
      unset($view->display_handler->handlers['footer']['area']);
    }
  }
}

