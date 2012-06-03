<?php

// TODO: Spellchecker field? What todo about spellchecker...

/**
 * Class for handling a view that gets its data not from the database, but from
 * a Solr server.
 */
class islandora_solr_views_query extends views_plugin_query {

  /**
   * Array of parameters for Solr query.
   */
  protected $params;

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

  public function build(&$view) {
    $view->init_pager();

    // Let the pager modify the query to add limits.
    $this->pager->query();

    // Set aliases of the fields.
    foreach ($view->field as $field_name => &$field) {
      //dsm($field);
      $field->field_alias = $field_name;
      $field->aliases['entity_type'] = 'entity_type';
    }

    // Add fields to the query so they will be shown in solr document.
    //dsm($this->params);
    $this->params['fl'] = array_keys($view->field);
  }

  /**
   * Executes the query and fills the associated view object with according
   * values.
   *
   * Values to set: $view->result, $view->total_rows, $view->execute_time,
   * $view->pager['current_page'].
   */
  public function execute(&$view) {
    //dsm($view);
    //dsm($this->params);
    try {
      $start = microtime(TRUE);
      
      // include common.inc
      module_load_include('inc', 'islandora_solr', 'includes/common');

      // Execute the search.
      $params = array();
      if (isset($this->params['q'])) {
        $params['q'] =  $this->params['q'];
      }

      $params['rows'] = $this->pager->options['items_per_page'];
      $params['start'] = $this->pager->current_page * $this->pager->options['items_per_page'];

      // If we display all items without pager.
      if ($params['rows'] == 0) {
        $params['rows'] = 1000000;
      }

      // Add fields.
      if (isset($this->params['fl'])) {
        // always return PID
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
        $params['sort'] = implode(',', $this->params['sort']);
        // sample sort parameters: q=video&sort=inStock asc, price desc
      }

      // by default return all results
      $query = '*:*';
      // by default, don't use dismax
      $dismax = NULL;

      // new query processor class
      $islandoraSolrQuery = new IslandoraSolrQueryProcessor();

      // check for dismax (not functional yet)
      if ($dismax != NULL) {
        $islandoraSolrQuery->solrDefType = $dismax;
        $params['defType'] = $dismax;
      }

      // add solr limit
      $islandoraSolrQuery->solrLimit = $this->pager->options['items_per_page'];
      // add solr start
      $islandoraSolrQuery->solrStart = max(0, $this->pager->current_page) * $islandoraSolrQuery->solrLimit;
      // add query (defaults to *:*)
      $islandoraSolrQuery->solrQuery = restore_slashes($query);
      // add solr parameters
      $islandoraSolrQuery->solrParams = $params;

      // excecute query
      $islandoraSolrQuery->executeQuery();
      
      // solr results
      $results = $islandoraSolrQuery->solrResult;
      $raw_response = $results->getRawResponse();
      $response_array = json_decode($raw_response, TRUE);
      dsm($response_array);
      
      // populate views results
      foreach ($response_array['response']['docs'] as $result_item) {
        // quick fix to deal with multivalue fields
        // @TODO: make output user configurable
        foreach ($result_item as $key => $value) {
          if (is_array($value)) {
            $result_item[$key] = implode(', ', $value);
          }
        }
        $view->result[] = (object) $result_item;
      }
      
      // Store response
      $this->islandora_solr_response = $response_array;

      // Store the results.
      $this->pager->total_items = $view->total_rows = $this->islandora_solr_response['response']['numFound']; // no object, but array
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
        drupal_set_message($msg, 'error');
      }
      $view->result = array();
      $view->total_rows = 0;
      $view->execute_time = 0;
      return;
    }
  }

  public function add_filter($type, $value, $exclude = FALSE) {
    $exclude_string = ($exclude) ? '-' : '';
    $this->params['filters'][] = $exclude_string . $type . ':' . $value;
  }

  public function add_filter_string($string) {
    $this->params['q.alt'][] = $string;
  }

  public function add_sort($field, $order) {
    //dsm('sort!!');
    $this->params['sort'][] = $field . ' ' . $order;
    //dsm($this->params);
  }

  // called by click sort
  public function add_orderby($table, $field = NULL, $order = 'ASC', $alias = '', $params = array()) {
    $this->params['sort'][] = $alias . ' ' . $order;
    dsm($this->params);
  }

  public function add_parameter($key, $value) {
    $this->params[$key] = $value;
  }

  public function add_field($table_alias, $field, $alias = '', $params = array()) {
    if (isset($table_alias[$field])) {
      return $table_alias[$field];
    }
  }

  public function ensure_table($table, $relationship) {
    if ($table == 'node') {
      return drupal_map_assoc(
        array('title', 'type',
            'path', 'path_alias', 'uid', 'name', 'created', 'changed',
            'last_comment_or_change', 'nid', 'status', 'promote', 'moderate',
            'sticky', 'tnid', 'translate', 'language', 'comment_count', 'tid',
            'vid', 'timestamp'
        ));
    }
    
    if ($table == 'islandora_solr_object') {
      return drupal_map_assoc(
        array('PID', 'dc.title'
        ));
    }
    
    
    
  }

  public function get_params() {
    return $this->params;
  }
  
  public function test() {
    return 'test works';
  }
}
