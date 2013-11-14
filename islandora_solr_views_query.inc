<?php

/**
 * @file
 * Views query functions. Collects query data, prepares it, runs the query and
 * add results to the view.
 */

// @codingStandardsIgnoreStart
// This Whole file is ignored due to classes and methods are not camelcase and it
// being called all over the place. TODO bring up to coding standards

// TODO: Spellchecker field? What todo about spellchecker...
/**
 * Class for handling a view that gets its data not from the database, but from
 * a Solr server.
 */
class islandora_solr_views_query extends views_plugin_query {

  /**
   * array of parameters for Solr query
   */
  protected $params = array();

  /**
   * Array of all encountered errors. Each of these is fatal, meaning that a
   * non-empty $errors property will result in an empty result being returned.
   *
   * @var array
   */
  protected $errors;

  /**
   * The names of all fields whose value is required by a handler. The format
   * follows the same as Search API field identifiers (parent:child).
   *
   * @var array
   */
  protected $fields;

  protected $islandora_solr_results;

  /**
   * Function build.
   */
  public function build(&$view) {
    $view->init_pager();

    // Let the pager modify the query to add limits.
    $this->pager->query();

    // Set aliases of the fields.
    foreach ($view->field as $field_name => &$field) {
      $field->field_alias = $field_name;
      $field->aliases['entity_type'] = 'entity_type';
    }

    // Add fields to the query so they will be shown in solr document.
    $this->params['fl'] = array_keys($view->field);
  }

  /**
   * Function execute.
   * 
   * Executes the query and fills the associated view object with according
   * values.
   *
   * Values to set: $view->result, $view->total_rows, $view->execute_time,
   * $view->pager['current_page'].
   */
  public function execute(&$view) {
    try {
      $start = microtime(TRUE);

      // Include common.inc.
      module_load_include('inc', 'islandora_solr', 'includes/common');

      // Execute the search.
      $params = array();

      $params['rows'] = $this->pager->options['items_per_page'];
      $params['start'] = $this->pager->current_page * $this->pager->options['items_per_page'];

      // If we display all items without pager.
      if ($params['rows'] == 0) {
        $params['rows'] = 1000000;
      }

      // Add fields.
      if (isset($this->params['fl'])) {
        // Always return PID.
        if (!in_array('PID', $this->params['fl'])) {
          $this->params['fl'][] = 'PID';
        }
        $params['fl'] = implode(',', $this->params['fl']);
      }

      // Add filters to parameters.
      if (isset($this->params['filters'])) {
        $params['fq'] = $this->params['filters'];
      }

      // Add sorting.
      if (isset($this->params['sort'])) {
        // Populate sorting parameters.
        foreach ($this->params['sort'] as $field => $order) {
          $params['sort'][] = $field . ' ' . $order;
        }
      }

      // Set query.
      $dismax = NULL;
      if (isset($this->params['q'])) {
        // Query.
        $query = $this->params['q'];

        // Dismax.
        if (isset($this->params['defType']) AND $this->params['defType'] == 'dismax') {
          $dismax = $this->params['defType'];
          // If no qf fields are specified in the requestHandler a default
          // list is supplied here for dismax searches.
          $params['qf'] = 'dc.title^5 dc.subject^2 dc.description^2 dc.creator^2 dc.contributor^1 dc.type';
        }
      }
      else {
        $query = '*:*';
      }

      // New query processor class.
      $islandora_solr_query = new IslandoraSolrQueryProcessor();

      // Check for dismax (not functional yet).
      if ($dismax != NULL) {
        $islandora_solr_query->solrDefType = $dismax;
        $params['defType'] = $dismax;
      }

      // Add solr limit.
      $islandora_solr_query->solrLimit = $this->pager->options['items_per_page'];
      // Add solr start.
      $islandora_solr_query->solrStart = $this->offset;
      // Add query (defaults to *:*).
      $islandora_solr_query->solrQuery = islandora_solr_restore_slashes($query);
      // Add solr parameters.
      $islandora_solr_query->solrParams = $params;

      // Excecute query.
      $islandora_solr_query->executeQuery(FALSE);

      // Solr results.
      $results = $islandora_solr_query->islandoraSolrResult;
      $object_results = $results['response']['objects'];

      // Populate views results.
      foreach ($object_results as $object_result) {
        $view->result[] = (object) $object_result['solr_doc'];
      }

      // Store response.
      $this->islandora_solr_response = $results;

      // Store the results.
      // No object, but Array.
      $view->total_rows = $this->islandora_solr_response['response']['numFound'];
      $this->pager->total_items = $this->islandora_solr_response['response']['numFound'];
      $this->pager->update_page_info();

      // We shouldn't use $results['performance']['complete'] here, since
      // extracting the results probably takes considerable time as well.
      $view->execute_time = microtime(TRUE) - $start;
    }
    catch (Exception $e) {
      $this->errors[] = $e->getMessage();
    }

    if ($this->errors) {
      foreach ($this->errors as $msg) {
        drupal_set_message(filter_xss($msg), 'error');
      }
      $view->result = array();
      $view->total_rows = 0;
      $view->execute_time = 0;
      return;
    }
  }

  /**
   * Function add_filter.
   */
  public function add_filter($type, $value, $exclude = FALSE) {
    $exclude_string = ($exclude) ? '-' : '';
    $this->params['filters'][] = $exclude_string . $type . ':' . $value;
  }

  /**
   * Function add_filter_string.
   */
  public function add_filter_string($string) {
    $this->params['q.alt'][] = $string;
  }

  /**
   * Function add_sort.
   */
  public function add_sort($field, $order) {
    // When sort filters are exposed, two sort calls are made for reasons
    // unknown, canceling each other out. This way the second call will override
    // the first, avoiding the issue for now.
    // @see http://drupal.org/node/1473522
    $this->params['sort'][$field] = $order;
  }

  /**
   * Function add_parameter.
   */
  public function add_parameter($key, $value) {
    $this->params[$key] = $value;
  }

  /**
   * Function add_field.
   */
  public function add_field($table_alias, $field, $alias = '', $params = array()) {
    if (isset($table_alias[$field])) {
      return $table_alias[$field];
    }
  }

  /**
   * Function ensure_table.
   */
  public function ensure_table($table, $relationship) {
    if ($table == 'islandora_solr') {
      return drupal_map_assoc(array('PID'));
    }
  }

  /**
   * Function get_params.
   */
  public function get_params() {
    return $this->params;
  }

}
// @codingStandardsIgnoreEnd
