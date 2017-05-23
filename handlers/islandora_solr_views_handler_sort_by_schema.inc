<?php

/**
 * @file
 * Views base handler for sort.
 */

// @codingStandardsIgnoreStart
// This Whole file is ignored due to classes and methods are not camelcase and it
// being called all over the place. TODO bring up to coding standards

/**
 * Class for sorting for a field.
 */
class islandora_solr_views_handler_sort_by_schema extends views_handler_sort {

  /**
   * Places the sort into the search parameters.
   */
  public function query() {
    $order = drupal_strtolower($this->options['order']);
    $this->query->add_sort($this->options['solr_field'], $order);
  }
  /**
   * Define new custom option.
   */
  function option_definition() {
    $options = parent::option_definition();
    $options['solr_field'] = array('default' => 'PID');
    return $options;
  }

  /**
   * Custom admin summary for field.
   */
  function admin_summary() {
    $internal_summary = parent::admin_summary();
    return check_plain((string) $this->options['solr_field']) . ' ' . check_plain(drupal_strtolower((string) $this->options['order'])) . ' ' . $internal_summary;
  }

  /**
   * Define form element for 'solr_field'.
   */
  function options_form(&$form, &$form_state) {
    $form['solr_field'] = array(
      '#title' => t('Solr field to use for sorting'),
      '#description' => t("Select the Solr field to use for sorting"),
      '#size' => 45,
      '#type' => 'textfield',
      '#autocomplete_path' => 'islandora_solr_views/autocomplete_luke/sortable',
      '#default_value' => !empty($this->options['solr_field']) ? $this->options['solr_field'] : 'PID',
      '#required' => TRUE,
    );

    parent::options_form($form, $form_state);
  }
}
// @codingStandardsIgnoreEnd
