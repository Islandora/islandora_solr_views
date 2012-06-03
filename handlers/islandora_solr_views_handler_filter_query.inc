<?php

/**
 * Query Parser filter field.
 */
class islandora_solr_views_handler_filter_query extends islandora_solr_views_handler_filter {
  function query() {
    // exposed filter: value is array (can be multivalued)
    if (is_array($this->value)) {
      if (!empty($this->value[0])) {
        $this->query->add_parameter('q', $this->value[0]);
      }      
    }
    // non-exposed filter: value is string
    elseif (!empty($this->value)) {
      $this->query->add_parameter('q', $this->value);
    }
  }
}
