<?php
// include common.inc for lesser_escape())
// @TODO: call this somewhere else?
module_load_include('inc', 'islandora_solr', 'includes/common');

class islandora_solr_views_handler_argument extends views_handler_argument {

  function query() {
    
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

}