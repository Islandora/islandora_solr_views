<?php

class islandora_solr_views_handler_filter extends views_handler_filter {
  function query() {
    if (!empty($this->value)) {
      $value = $this->value;
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
      '#title' => t($this->definition['title']),
      '#default_value' => $this->value,
    );
  }
}