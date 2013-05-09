<?php

/**
 * @file
 * Views handler for a lucene query filter.
 */

/**
 * Lucene Query Parser filter field.
 */
class islandora_solr_views_handler_filter_query_lucene extends islandora_solr_views_handler_filter {
  function query() {
    // Exposed filter: value is array (can be multivalued).
    if (is_array($this->value)) {
      if (!empty($this->value[0])) {
        $this->query->add_parameter('q', $this->value[0]);
      }
    }
    // Non-exposed filter: value is string.
    elseif (!empty($this->value)) {
      $this->query->add_parameter('q', $this->value);
    }
  }
}
