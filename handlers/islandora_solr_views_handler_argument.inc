<?php
// include common.inc for lesser_escape())
// @TODO: call this somewhere else?
module_load_include('inc', 'islandora_solr', 'includes/common');

class islandora_solr_views_handler_argument extends views_handler_argument {

  function query() {
    //$this->query->add_where(0, "$this->table_alias.$this->real_field", $this->argument);
    
//    dsm($this->value);
//    dsm($this->argument);
//    dsm($this->field);
//    dsm($this->table_alias);
//    dsm($this->real_field);
    
    if (!empty($this->argument)) {
      $value = $this->argument;
      
      $value = lesser_escape($value);
      
      $fieldname = $this->field;

      if (is_array($value)) {
        $values = array_filter($value);
        // Ensure that some values have been selected.
        if (!empty($values)) {
          $this->query->add_filter($fieldname, '(' . implode('OR', $values) . ')');
        }
        return;
      }
      $this->query->add_filter($fieldname, $value);
    }
  }

//  /**
//   * Provide a simple textfield for equality
//   */
//  function value_form(&$form, &$form_state) {
//    $form['value'] = array(
//      '#type' => 'textfield',
//      '#title' => t($this->definition['title']),
//      '#default_value' => $this->value,
//    );
//  }
//  
//  /**
//   * Provide default options for exposed filters.
//   */
//  function expose_options() {
//    $this->options['expose']['identifier'] = strtolower( preg_replace('/[^A-Za-z0-9]/', '_', $this->options['id']) );
//  }

}