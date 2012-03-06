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
      'entity type' => 'node',
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

    // Get the list of the fields in idex directly from Solr.
    //$solr = apachesolr_get_solr();
    //$solr_fields = $solr->getFields(1);
    $solr_fields = array('mods_title_s' => 'mods_title_s');
    foreach ($solr_fields as $solr_field_name => $solr_field) {

      // We do not allow to display 'sort_*' fields.
      if (strpos($solr_field_name, 'sort_') === 0) {
        continue;
      }

//      $field_type = $solr_field->type;
      $field_type = 'field_type';

      //$field_names = apachesolr_views_field_names($solr_field_name);

      $data[$islandora_solr_base_table][$solr_field_name] = array(
        'title' => t('Mods Title'),
        'help' => t('Mods Title help'),
        'field' => array(
          'handler' => 'islandora_solr_views_handler_field',
          'click sortable' => TRUE,
        ),
        // Information for accepting a nid as an argument
//        'argument' => array(
//          'handler' => 'views_handler_argument_node_nid',
//          'name field' => 'title', // the field to display in the summary.
//          'numeric' => TRUE,
//          'validate type' => 'nid',
//        ),
        'filter' => array(
          'handler' => 'islandora_solr_views_handler_filter',
        ),
        'sort' => array(
          'handler' => 'islandora_solr_views_handler_sort',
        ),
      );
    }

  }
//  dsm($data);
  return $data;
}
