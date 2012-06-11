<?php

/**
 * Class for sorting for a field.
 */
class islandora_solr_views_handler_sort extends views_handler_sort {

  /**
   * Places the sort into the search parameters.
   */
  public function query() {

    $order = strtolower($this->options['order']);
    $this->query->add_sort($this->real_field, $order);
  }
}
