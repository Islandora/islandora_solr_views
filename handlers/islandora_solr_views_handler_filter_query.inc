<?php

/**
 * Query Parser filter field.
 */
class islandora_solr_views_handler_filter_query extends islandora_solr_views_handler_filter {
  function query() {
    // @TODO: not sure why this is an array on exposed filter
    if (is_array($this->value)) {
      if (!empty($this->value[0])) {
        $this->query->add_parameter('q', $this->value[0]);
      }      
    }
    elseif (!empty($this->value)) {
      $this->query->add_parameter('q', $this->value);
    }
  }
  
}
