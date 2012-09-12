<?php

/**
 * @file
 * Views filter handler base
 */

// include common.inc for lesser_escape())
// @TODO: call this somewhere else?
module_load_include('inc', 'islandora_solr', 'includes/common');

class islandora_solr_views_handler_filter extends views_handler_filter {
  function query() {
    if (!empty($this->value)) {
      $value = $this->value;

      $value = lesser_escape($value);

      $fieldname = $this->field;

      if (is_array($value)) {
        $values = array_filter($value);
        // Ensure that some values have been selected.
        if (!empty($values)) {
          $this->query->add_filter($fieldname, '(' . implode('OR', $values) . ')');
        }
        return;
      }
      $this->query->add_filter($fieldname, $value);
    }
  }

  /**
   * Provide a simple textfield for equality
   */
  function value_form(&$form, &$form_state) {
    $form['value'] = array(
      '#type' => 'textfield',
      '#title' => check_plain($this->definition['title']),
      '#default_value' => $this->value,
    );
  }

  /**
   * Provide default options for exposed filters.
   */
  function expose_options() {
    $this->options['expose']['identifier'] = drupal_strtolower( preg_replace('/[^A-Za-z0-9]/', '_', $this->options['id']) );
  }

}