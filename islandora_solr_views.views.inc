<?php

/*
 * Load files with base classes of the contained classes.
 */

/**
 * Implementation of hook_views_plugins().
 */
function islandora_solr_views_views_plugins() {
  return array(
    'module' => 'islandora_solr_views',
    'query' => array(
      'islandora_solr_views_query' => array(
        'title' => t('Islandora Solr Query'),
        'help' => t('Query that allows you to search with Islandora Solr.'),
        'handler' => 'islandora_solr_views_query',
        'parent' => 'views_query',
      ),
    ),
  );

}

/**
 * Implements hook_islandora_solr_entities().
 *
 * This is done for apachesolr_search module.
 */
function islandora_solr_views_islandora_solr_entities() {
  return array(
    'object' => array(
      'name' => 'Islandora object',
      'base_field' => 'PID',
    ),
  );
}



/**
 * Implementation of hook_views_data().
 */
function islandora_solr_views_views_data() {
  // set base variables
  $base_field = 'PID';
  $base_table = 'islandora_solr';
  $data[$base_table]['table']['group'] = t('Islandora Solr');

  $data[$base_table]['table']['base'] = array(
    'query class' => 'islandora_solr_views_query',
    'title' => t('Islandora Solr'),
    'help' => t('Searches the Islandora Solr index.'),
    'field' => $base_field,
  );


  // Get the list of the fields in index directly from Solr.
  $luke = islandora_solr_get_luke();
  $solr_fields = $luke['fields'];

  // always add score handlers    
  $data[$base_table]['score'] = array(
      'title' => t('Score'),
      'help' => t('Relevancy score'),
      'field' => array(
        'handler' => 'islandora_solr_views_handler_field',
        'click sortable' => TRUE,
      ),
      'argument' => array(
        'handler' => 'islandora_solr_views_handler_argument',
      ),
      'filter' => array(
        'handler' => 'islandora_solr_views_handler_filter',
      ),
      'sort' => array(
        'handler' => 'islandora_solr_views_handler_sort',
      ),
  );

  // loop over all solr fields
  foreach ($solr_fields as $solr_field_name => $solr_field) {

    // We do not allow to display 'sort_*' fields.
    if (strpos($solr_field_name, 'sort_') === 0) {
      continue;
    }
    // set luke field variables
    $field_type = $solr_field['type'];
    $field_schema = $solr_field['schema'];
    $field_dynamicbase = isset($solr_field['dynamicBase']) ? $solr_field['dynamicBase'] : NULL;

    // set field handlers
    $field = array ();
    $field['title'] = $solr_field_name;
    $field['help'] = t('Type') . ': ' .$field_type;
    // field handler
    $field['field']['handler'] = 'islandora_solr_views_handler_field';
    // check if sortable
    if (strstr($field_schema, "I") != FALSE AND strstr($field_schema, "M") == FALSE) {
      $field['field']['click sortable'] = TRUE;
    }
    // argument handler
    $field['argument'] = array(
      'handler' => 'islandora_solr_views_handler_argument',
    );
    // filter handler
    $field['filter'] = array(
      'handler' => 'islandora_solr_views_handler_filter',
    );
    // sortable handler
    // check if sortable
    if (strstr($field_schema, "I") != FALSE AND strstr($field_schema, "M") == FALSE) {
      $field['sort'] = array(
        'handler' => 'islandora_solr_views_handler_sort',
      );
    }
    // add array
    $data[$base_table][$solr_field_name] = $field;

  }

  return $data;
}


