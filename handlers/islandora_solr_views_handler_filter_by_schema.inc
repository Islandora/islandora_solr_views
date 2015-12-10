<?php

/**
 * @file
 * Views filter handler base.
 */

// @codingStandardsIgnoreStart
// This Whole file is ignored due to classes and methods are not camelcase and it
// being called all over the place. TODO bring up to coding standards
class islandora_solr_views_handler_filter_by_schema extends views_handler_filter {

  function query() {
    if (!empty($this->value) && !empty($this->options['solr_field'])) {
      $value = $this->value;
      module_load_include('inc', 'islandora_solr', 'includes/utilities');
      // Only escape if 'value_type' is disabled to allow range queries
      // and other non string ones.
      if (!$this->options['value_type']) {
        $value = islandora_solr_lesser_escape($value);
      }
      $exclude = isset($this->operator) && $this->operator === '!=';
      if (is_array($value)) {
        $values = array_filter($value);
        // Ensure that some values have been selected.
        if (!empty($values)) {
          $this->query->add_filter($this->options['solr_field'], '(' . implode('OR', $values) . ')', $this->options['group'], $exclude);
        }
        return;
      }
      $this->query->add_filter($this->options['solr_field'], $value, $this->options['group'], $exclude);
    }
  }
  /**
   * Define new custom options.
   */
  function option_definition() {
    $options = parent::option_definition();
    $options['solr_field'] = array('default' => 'PID');
    $options['value_type'] = array('default' => 0);
    // There is a bug in Views that makes this option required,
    // even when not exposed.
    $options['expose']['identifier'] = array('default' => 'solrfilter');

    return $options;
  }

  function admin_summary() {
    return check_plain((string) $this->options['solr_field']) . check_plain((string) $this->operator) . ' ' . check_plain((string) $this->value);
  }

  /**
   * Define custom form elements to match options.
   */
  function options_form(&$form, &$form_state) {
    // Add an option to allow non string filters like [* TO NOW]
    $form['value_type'] = array(
      '#type' => 'checkbox',
      '#title' => t("Don't escape filter value for Solr"),
      '#description' => t('Enable filter value to be passed without escaping. Useful for e.g [* TO NOW]'),
      '#default_value' => !empty($this->options['value_type']) ? $this->options['value_type'] : 0,
      '#weight' => 11,
    );
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
    parent::options_form($form, $form_state);
    // Modify the title for 'value'
    $form['value']['#title'] = t("Filter value for Solr field");
    $form['value']['#weigth'] = 10;

  }
  /**
   * Provide a simple textfield for equality.
   */
  function value_form(&$form, &$form_state) {
    $which = 'all';
    $form['value'] = array(
      '#type' => 'textfield',
      '#title' => check_plain($this->definition['title']),
      '#default_value' => $this->value,
    );
    if (!empty($form['operator'])) {
      $source = ($form['operator']['#type'] == 'radios') ? 'radio:options[operator]' : 'edit-options-operator';
    }
    if (!empty($form_state['exposed'])) {
      $identifier = $this->options['expose']['identifier'];

      if (empty($this->options['expose']['use_operator']) || empty($this->options['expose']['operator_id'])) {
        // Exposed and locked.
        $which = in_array($this->operator, $this->operator_values(1)) ? 'value' : 'none';
      }
      else {
        $source = 'edit-' . drupal_html_id($this->options['expose']['operator_id']);
      }
    }
    if (!empty($form_state['exposed']) && !isset($form_state['input'][$identifier])) {
      $form_state['input'][$identifier] = $this->value;
    }
  }

  /**
   * Provide default options for exposed filters.
   */
  function expose_options() {
    parent::expose_options();
    $this->options['expose']['identifier'] = drupal_strtolower(preg_replace('/[^A-Za-z0-9]/', '_', $this->options['id']));
  }

  /**
   * Don't allow exposure for not filtered values.
   */
  function can_expose() {
    return ($this->options['value_type'] == 0);
  }

  /**
   * Validation handler, the parent:: fails on disabling 'expose'.
   */
  function options_validate(&$form, &$form_state) {
    $this->operator_validate($form, $form_state);
    $this->value_validate($form, $form_state);
    if (isset($form_state['values']['expose_button']['checkbox']['checkbox']) && ($form_state['values']['expose_button']['checkbox']['checkbox'] == 1) && !$this->is_a_group()) {
      $this->expose_validate($form, $form_state);
    }
    if ($this->is_a_group()) {
      $this->build_group_validate($form, $form_state);
    }
  }

  function operators() {
    $operators = array(
      '=' => array(
        'title' => t('Is equal to'),
        'short' => t('='),
        'values' => 1,
      ),
      '!=' => array(
        'title' => t('Is not equal to'),
        'short' => t('!='),
        'values' => 1,
      ),
    );
    return $operators;
  }

  /**
   * Build strings from the operators() for 'select' options.
   */
  function operator_options($which = 'title') {
    $options = array();
    foreach ($this->operators() as $id => $info) {
      $options[$id] = $info[$which];
    }

    return $options;
  }

  function operator_values($values = 1) {
    $options = array();
    foreach ($this->operators() as $id => $info) {
      if (isset($info['values']) && $info['values'] == $values) {
        $options[] = $id;
      }
    }

    return $options;
  }

  public function expose_form(&$form, &$form_state) {
    parent::expose_form($form, $form_state);
    if (empty($form['expose']['identifier']['#default_value'])) {
      $form['expose']['identifier']['#default_value'] = $this->options['field'];
    }
    if (empty($form['expose']['label']['#default_value'])) {
      $form['expose']['label']['#default_value'] = $this->definition['title'];
    }
    if (empty($form['ui_name']['#default_value'])) {
      $form['ui_name']['#default_value'] = $this->definition['title'];
    }
  }
}
// @codingStandardsIgnoreEnd