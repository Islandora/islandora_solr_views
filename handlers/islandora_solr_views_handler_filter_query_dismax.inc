<?php

/**
 * Query Parser filter field.
 */
class islandora_solr_views_handler_filter_query_dismax extends islandora_solr_views_handler_filter_query_lucene {
  
  function query() {
    parent::query();
    $this->query->add_parameter('defType', 'dismax');
  }
}
