<?php

/**
 * @file
 * Views handler for a dismax query filter
 */

/**
 * Query Parser filter field.
 */
class islandora_solr_views_handler_filter_query_dismax extends islandora_solr_views_handler_filter_query_lucene {

  // @TODO add requestHandler option (dropdown)

  function query() {
    parent::query();
    $this->query->add_parameter('defType', 'dismax');
  }
}
