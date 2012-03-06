<?php

class islandora_solr_views_handler_field extends views_handler_field {

  function get_value($values, $field = NULL) {
//    dsm($values);
//    dsm($field);
    $alias = isset($field) ? $this->aliases[$field] : $this->field_alias;
    if (isset($values->{$alias})) {
      return $values->{$alias};
    }
  }

}


  /**
   * Get the value that's supposed to be rendered.
   *
   * This api exists so that other modules can easy set the values of the field
   * without having the need to change the render method as well.
   *
   * @param $values
   *   An object containing all retrieved values.
   * @param $field
   *   Optional name of the field where the value is stored.
   */
//  function get_value($values, $field = NULL) {
//    $alias = isset($field) ? $this->aliases[$field] : $this->field_alias;
//    if (isset($values->{$alias})) {
//      return $values->{$alias};
//    }
//  }