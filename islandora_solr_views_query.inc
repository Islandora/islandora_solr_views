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

  /**
   * An array of sections of the WHERE query. Each section is in itself
   * an array of pieces and a flag as to whether or not it should be AND
   * or OR.
   */
  protected $where = array();

  /**
   * The default operator to use when connecting the WHERE groups. May be
   * AND or OR.
   */
  protected $group_operator = 'AND';

  protected $islandora_solr_results;

  /**
   * Function build.
   */
  public function build(&$view) {
    $view->init_pager();
    // Let the pager modify the query to add limits.
    $this->pager->query();
    
    // Real Solr fields mapper
    $solr_fields = array();
    // Set aliases of the fields.
    // Since we our real Solr field will not match always the $field_name,
    // we will have to build our own naming structure.
    foreach ($view->field as $field_name => &$field) {
      $solr_field_name_to_use = isset($field->options['solr_field']) ? $field->options['solr_field'] : $field->real_field;
      if (isset($solr_fields[$solr_field_name_to_use])) {
        $cardinality = array_push($solr_fields[$solr_field_name_to_use], $field_name);
      }
      else {
        $solr_fields[$solr_field_name_to_use] = array($field_name);
        $cardinality = 1;
      }
      $field->field_alias = $cardinality == 1 ? $solr_field_name_to_use : $solr_field_name_to_use . "_" . ($cardinality-1); 
      $field->aliases['entity_type'] = 'entity_type';
    }
    // Add fields to the query so they will be shown in solr document.
    $this->params['fl'] = array_keys($solr_fields);
  }

  /**
   * Function execute.
   *
   * Executes the query and fills the associated view object with according
   * values.
   *
   * @global IslandoraSolrQueryProcessor $_islandora_solr_queryclass
   *   The IslandoraSolrQueryProcessor object which includes the current query
   *   settings and the raw Solr results.
   *
   * Values to set: $view->result, $view->total_rows, $view->execute_time,
   * $view->pager['current_page'].
   */
  public function execute(&$view) {
    try {
      // New query processor class.
      $islandora_solr_query = new IslandoraSolrQueryProcessor();
      // Only enable facets if needed. Makes queries faster
      if ($this->options['enable_facets']) {
        global $_islandora_solr_queryclass;
        // Set our famous global so we can enable the facet block
        $_islandora_solr_queryclass = $islandora_solr_query;
      }
        
      $start = microtime(TRUE);
      // Include common.inc.
      module_load_include('inc', 'islandora_solr', 'includes/common');

      // Execute the search.
      $params = array();
      $params['rows'] = $this->pager->options['items_per_page'];

      // If we display all items without pager.
      if ($params['rows'] == 0) {
        $params['rows'] = 1000000;
        if (!isset($this->offset)) {
          $this->offset = 0; 
        }
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
        // Need to see what filter group operations we may have present.
        $group_ops = $this->where;
        $constructed_group_params = array();
        foreach ($this->params['filters'] as $group => $filter) {
          // Filters that need to be grouped will come in as arrays.
          if (is_array($filter)) {
            $group_string = implode(" {$group_ops[$group]['type']} ", $filter);
            $constructed_group_params[$group] = format_string("(!group_string)", array('!group_string' => $group_string));
          }
          else {
            $params['f'][] = $filter;
          }
        }
        $params['f'][] = implode(" {$this->group_operator} ", $constructed_group_params);
      }

      // Add sorting.
      if (isset($this->orderby)) {
        module_load_include('inc', 'islandora_solr', 'includes/utilities');
        // Populate sorting parameter as one string.
         $params['sort'] = array_map(function($item) {
          return $item['field'] .' '. $item['direction'];
        }, $this->orderby);
      }

      // Set query.
      $dismax = NULL;
      if (isset($this->params['q'])) {
        // Query.
        $query = $this->params['q'];

        // Dismax.
        if (isset($this->params['defType']) AND $this->params['defType'] == 'dismax') {
          $dismax = $this->params['defType'];
        }
      }
      else {
        $query = '*:*';
      }


      // Check for dismax (not functional yet).
      if ($dismax != NULL) {
        $islandora_solr_query->solrDefType = $dismax;
        $params['defType'] = $dismax;
      }
      // Add query (defaults to *:*).
      $islandora_solr_query->buildQuery($query, $params);
      // Add 'fl' to solrParams since the query builder does not use them.
      $islandora_solr_query->solrParams['fl'] = $params['fl'];
      // Disable or enable facets depending on user option.
      $islandora_solr_query->solrParams['facet'] = $this->options['enable_facets'] ? 'true' : 'false';
      // Add solr limit.
      $islandora_solr_query->solrLimit = $params['rows'];
      // Add solr start.
      $islandora_solr_query->solrStart = $this->offset;
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

      // Store the results, need to subtract the start from the number found
      // to account for offsets.
      $view->total_rows = $this->islandora_solr_response['response']['numFound'] - $this->pager->options['offset'];
      $this->pager->total_items = $this->islandora_solr_response['response']['numFound'] - $this->pager->options['offset'];
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
  function option_definition() {
    $options = parent::option_definition();
    $options['enable_facets'] = array('default' => FALSE);
    return $options;
  }

  function options_form(&$form, &$form_state) {

    $form['enable_facets'] = array(
      '#type' => 'checkbox',
      '#title' => t('Enable Facets'),
      '#default_value' => $this->options['enable_facets'],
      '#description' => t('This option enables facet processing and global Solr variable to display block'),
      '#required' => FALSE,
    );
  }
  /**
  * Function add_filter.
  */
  public function add_filter($type, $value, $group, $exclude = FALSE) {
    module_load_include('inc', 'islandora_solr', 'includes/utilities');
    $exclude_string = ($exclude) ? '-' : '';
    if ($group) {
      $this->params['filters'][$group][] = $exclude_string . $type . ':' . $value;
    }
    else {
      $this->params['filters'][] = $exclude_string . $type . ':' . $value;

    }
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
    //$this->orderby[$field] = $order;
    $this->orderby[] = array(
      'field' => $field,
      'direction' => drupal_strtolower($order)
    );
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
