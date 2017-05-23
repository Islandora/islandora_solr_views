<?php

/**
 * @file
 * Handles Luke autocomplete callback for Solr Views module.
 */

/**
 * Admin autocomplete callback which returns solr fields from Luke.
 *
 * @param string $fieldtype
 *   Type of Solr field to filter against.
 *   Accepted values are 'all'|'date'|'sortable'|'displayable'
 *
 * @param string $string
 *   String filled out in the autocomplete textfield.
 *
 * @return json
 *   A json array containing the Solr luke values that contain the given string.
 */
function _islandora_solr_views_autocomplete_luke($fieldtype = 'all', $string = '') {
  module_load_include('inc', 'islandora_solr', 'includes/luke');
  $luke = islandora_solr_get_luke();
  $result = array();
  if ($fieldtype != 'all') {
    $fieldsbytype = array_filter(
      $luke['fields'],
      function ($term) use ($fieldtype) {
        switch ($fieldtype) {
          case 'date':
            return ($term['type'] == 'date');

          case 'sortable':
            return (strpos($term['schema'], "I") !== FALSE && strpos($term['schema'], "M") === FALSE);

          case 'displayable':
            return (strpos($term['schema'], "S") !== FALSE);
        }
      }
    );
  }
  else {
    $fieldsbytype = $luke['fields'];
  }
  foreach ($fieldsbytype as $term => $value) {
    if (stripos($term, $string) !== FALSE) {
      // Search case insensitive, but keep the case on replace.
      $term_str = preg_replace("/$string/i", "<strong>\$0</strong>", $term);

      // Add strong elements to highlight the found string.
      $result[$term] = $term_str . '<strong style="position: absolute; right: 5px;">(' . $value['type'] . ')</strong>';
    }
  }
  // Sort alphabetically.
  ksort($result);

  drupal_json_output($result);
  exit();
}
