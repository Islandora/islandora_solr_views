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
 * Implementation of hook_views_data().
 */
function islandora_solr_views_views_data() {
  
  foreach (module_invoke_all('islandora_solr_entities') as $base_table => $definition) {
    $name = $definition['name'];
    $base_field = $definition['base_field'];
    $islandora_solr_base_table = 'islandora_solr_' . $base_table;
    $data[$islandora_solr_base_table]['table']['group'] = t('Islandora Solr');

    $data[$islandora_solr_base_table]['table']['base'] = array(
      'query class' => 'islandora_solr_views_query',
      'title' => t('Islandora Solr @name', array('@name' => $name)),
      'help' => t('Searches the site with the Islandora Solr search engine for @name', array('@name' => $name)),
      'field' => $base_field,
      //'entity type' => 'fedora object',  // do we really need this?
    );

//    $data[$apachesolr_base_table]['text'] = array(
//      'title' => t('Search'),
//      'help' => t('Searches the content with Solr'),
//      'filter' => array(
//        'handler' => 'apachesolr_views_handler_filter_search',
//      ),
//    );

//    $data[$apachesolr_base_table]['nid'] = array(
//      'title' => t('Nid'),
//      'help' => t('The node ID of the node.'),
//      'field' => array(
//        'handler' => 'views_handler_field_node',
//        'click sortable' => TRUE,
//      ),
//      'sort' => array(
//        'handler' => 'apachesolr_views_handler_sort',
//      ),
//    );

    // Get the list of the fields in index directly from Solr.
    //$solr = apachesolr_get_solr();
    //$solr_fields = $solr->getFields(1);

    $luke = islandora_solr_get_luke();
    
    $solr_fields = $luke['fields'];
    
    // always add score handlers    
    $data[$islandora_solr_base_table]['score'] = array(
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
    
    foreach ($solr_fields as $solr_field_name => $solr_field) {

      // We do not allow to display 'sort_*' fields.
      if (strpos($solr_field_name, 'sort_') === 0) {
        continue;
      }

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
      $data[$islandora_solr_base_table][$solr_field_name] = $field;
      
    }

  }
  return $data;
}


