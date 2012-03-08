<?php

/**
 * Class for sorting for a field.
 */
class islandora_solr_views_handler_sort extends views_handler_sort {

  /**
   * Places the sort into the search parameters.
   */
  public function query() {
    // Find out why apachesolr module adds sort_* fields
    //  to index and use them if needed.
    //dsm($this->options);
    //dsm($this->real_field);
    $order = strtolower($this->options['order']);
    $this->query->add_sort($this->real_field, $order);
  }

}
