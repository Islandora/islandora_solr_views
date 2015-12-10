<?php

/**
 * @file
 * Views base handler for arguments
 */

// @codingStandardsIgnoreStart
// This Whole file is ignored due to classes and methods are not camelcase and it
// being called all over the place. TODO bring up to coding standards
class islandora_solr_views_handler_argument_by_schema extends views_handler_argument {
  /**
   * Function query.
   */
  function query($group_by = FALSE) {
    if (!empty($this->argument) && !empty($this->options['solr_field'])) {
      $value = islandora_solr_lesser_escape($this->argument);
      $solr_field = $this->options['solr_field'];
      module_load_include('inc', 'islandora_solr', 'includes/utilities');
      $group = isset($this->options['group']) ? $this->options['group'] : FALSE;
      if (is_array($value)) {
        $values = array_filter($value);
        // Ensure that some values have been selected.
        if (!empty($values)) {
          $this->query->add_filter($solr_field, '(' . implode('OR', $values) . ')', $group);
        }
        return;
      }
      if (!empty($value)) {
        $this->query->add_filter($solr_field, $value, $group);
      }
    }
  }

  /**
   * Define custom option for our solr field.
   */
  function option_definition() {
    $options = parent::option_definition();
    $options['solr_field'] = array('default' => 'PID');
    return $options;
  }

  /**
   * Define form element for real solr field.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['solr_field'] = array(
      '#prefix' => '<div class="clearfix">',
      '#suffix' => '</div>',
      '#title' => t('Solr field to use'),
      '#description' => t("Select the Solr field to use"),
      '#size' => 45,
      '#type' => 'textfield',
      '#autocomplete_path' => 'islandora_solr_views/autocomplete_luke/all',
      '#default_value' => !empty($this->options['solr_field']) ? $this->options['solr_field'] : 'PID',
      '#required' => TRUE,
      '#weight' => -10,
    );
  }
}
// @codingStandardsIgnoreEnd
