<?php

/**
 * @file
 * Contains all blocks related functionality.
 */

/**
 * Implements hook_islandora_solr_query_blocks().
 */
function islandora_solr_views_islandora_solr_query_blocks() {
  // Note: As in the examples below, it is valid to specify
  // *either* a class and method *or* a form.  The latter implies no
  // class needs to be instantiated.
  return array(
    'views_facets' => array(
      'name' => t('Islandora facets for Solr Views'),
      'module' => 'islandora_solr_views',
      'file' => 'includes/blocks.inc',
      'class' => 'IslandoraSolrResultsView',
      'function' => 'displayFacets',
      'form' => NULL,
    ),
  );
}

class IslandoraSolrResultsView extends IslandoraSolrResults {};
