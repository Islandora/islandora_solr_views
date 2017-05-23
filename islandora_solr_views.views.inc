<?php

/**
 * @file
 * This file contains views definitions. Defines views and data/field types.
 */


/**
 * Implements hook_views_plugins().
 */
function islandora_solr_views_views_plugins() {
  return array(
    'module' => 'islandora_solr_views',
    'query' => array(
      'islandora_solr_views_query' => array(
        'title' => t('Islandora Solr Query'),
        'help' => t('Query that allows you to search Islandora Solr.'),
        'handler' => 'islandora_solr_views_query',
        'parent' => 'views_query',
      ),
    ),
  );

}

/**
 * Implements hook_views_data().
 */
function islandora_solr_views_views_data() {
  // Set base variables.
  module_load_include('inc', 'islandora_solr', 'includes/luke');
  $base_field = 'PID';
  $base_table = 'islandora_solr';
  $data[$base_table]['table']['group'] = t('Islandora Solr');

  $data[$base_table]['table']['base'] = array(
    'query class' => 'islandora_solr_views_query',
    'title' => t('Islandora Solr'),
    'help' => t('Searches the Islandora Solr index.'),
    'field' => $base_field,
  );
  // Always add score handlers.
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

  // Include object manage link.
  $data[$base_table]['islandora_manage'] = array(
    'title' => t('Manage link'),
    'help' => t('Provide a simple link to manage the content.'),
    'field' => array(
      'handler' => 'islandora_solr_views_handler_field_manage',
    ),
  );

  // Include a lucene query parser filter (q).
  $data[$base_table]['query_lucene'] = array(
    'title' => t('Query Parser (Lucene)'),
    'help' => t('Query Parser field to perform Lucene keyword searches.'),
    'filter' => array(
      'handler' => 'islandora_solr_views_handler_filter_query_lucene',
    ),
  );

  // Include a dismax query parser filter (q).
  $data[$base_table]['query_dismax'] = array(
    'title' => t('Query Parser (DisMax)'),
    'help' => t('Query Parser field to perform DisMax keyword searches.'),
    'filter' => array(
      'handler' => 'islandora_solr_views_handler_filter_query_dismax',
    ),
  );

  // Get the list of the fields in index directly from Solr.
  $luke = islandora_solr_get_luke();
  $solr_fields = $luke['fields'];

  // Create template array for different supported field
  // types.
  $solr_fields_types_to_create = array(
    'date' => array(
      'create' => FALSE,
      'title' => t('Date Solr field'),
      'help' => t('Any Solr field of type Date'),
    ),
    'sortable' => array(
      'create' => FALSE,
      'title' => t('Sortable Solr field'),
      'help' => t('Any Solr field that can be used to sort'),
    ),
    'general' => array(
      'create' => count($solr_fields) > 0 ? TRUE : FALSE,
      'title' => t('General Solr field'),
      'help' => t('Any Solr field that can be used to query, filter or as context argument'),
    ),
  );
  // Loop over all solr fields. We will group by types
  // instead of creating individual fields.
  foreach ($solr_fields as $solr_field_name => $solr_field) {
    // Set luke field variables.
    $field_type = $solr_field['type'];
    $field_schema = $solr_field['schema'];
    if ($field_type == 'date') {
      $solr_fields_types_to_create['date']['create'] = TRUE;
    }
    if (strstr($field_schema, "I") != FALSE AND strstr($field_schema, "M") == FALSE) {
      $solr_fields_types_to_create['sortable']['create'] = TRUE;
    }
  }
  // Create actual fields grouped by schema type.
  foreach ($solr_fields_types_to_create as $type => $field_data) {
    if ($field_data['create']) {
      $field = array();
      $field['title'] = $field_data['title'];
      $field['help'] = $field_data['help'];
      if ($type == 'date') {
        $field['field'] = array(
          'handler' => 'islandora_solr_views_handler_field_date',
        );
      }
      else {
        $field['field'] = array(
          'handler' => 'islandora_solr_views_handler_field_by_schema',
        );
      }
      $field['filter'] = array(
        'handler' => 'islandora_solr_views_handler_filter_by_schema',
      );
      if ($type == 'sortable') {
        $field['sort'] = array(
          'handler' => 'islandora_solr_views_handler_sort_by_schema',
        );
      }
      if ($type == 'general') {
        $field['argument'] = array(
          'handler' => 'islandora_solr_views_handler_argument_by_schema',
        );
      }
      $data[$base_table][$type] = $field;
    }
  }

  // Add our collection counting goodness.
  $data[$base_table]['solr_collection_count'] = array(
    'title' => 'Collection count',
    'help' => t('This shows the number of immediate child objects in the collection.'),
    'field' => array(
      'handler' => 'IslandoraSolrViewsHandlerCollectionCount',
    ),
  );
  return $data;
}
