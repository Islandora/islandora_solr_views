<?php

/**
 * @file
 * Views base handler for field.
 */

// @codingStandardsIgnoreStart
// This Whole file is ignored due to classes and methods are not camelcase and it
// being called all over the place. TODO bring up to coding standards

class islandora_solr_views_handler_field_by_schema extends islandora_solr_views_handler_field {


  /**
   * Define new options for additional solr field.
   */
  function option_definition() {
    $options = parent::option_definition();
    $options['solr_field'] = array('default' => 'PID');
    return $options;
  }

  /**
   * Define form element for 'link to object' option and 'solr_field'.
   */
  function options_form(&$form, &$form_state) {

    $form['solr_field'] = array(
      '#title' => t('Solr field to use'),
      '#description' => t("Select the Solr field to use"),
      '#size' => 45,
      '#type' => 'textfield',
      '#autocomplete_path' => 'islandora_solr_views/autocomplete_luke/displayable',
      '#default_value' => !empty($this->options['solr_field']) ? $this->options['solr_field'] : 'PID',
      '#required' => TRUE,
    );

    parent::options_form($form, $form_state);
  }

  function ui_name($short = FALSE) {
    return $this->get_field(parent::ui_name($short)) . ' ' . $this->options['solr_field'];
  }


}
// @codingStandardsIgnoreEnd
