<?php

class islandora_solr_views_handler_field extends views_handler_field {

  function init(&$view, &$options) {
    parent::init($view, $options);
    // Don't add the additional fields to groupby
    if (!empty($this->options['link_to_object'])) {
      //dsm($this->additional_fields);
      $this->additional_fields['PID'] = array('table' => 'islandora_solr_object', 'field' => 'PID');
      //dsm($this->additional_fields);
    }
       
  }
  
  function get_value($values, $field = NULL) {
//    dsm($values);
//    dsm($field);
    $alias = isset($field) ? $this->aliases[$field] : $this->field_alias;
    if (isset($values->{$alias})) {
      return $values->{$alias};
    }
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['link_to_object'] = TRUE;
    return $options;
  }
  
  /**
   * Provide link to object option
   */
  function options_form(&$form, &$form_state) {
    $form['link_to_object'] = array(
      '#title' => t('Link this field to the original piece of content'),
      '#description' => t("Enable to override this field's links."),
      '#type' => 'checkbox',
      '#default_value' => !empty($this->options['link_to_object']),
    );

    parent::options_form($form, $form_state);
  }


  /**
   * Render whatever the data is as a link to the object.
   *
   * Data should be made XSS safe prior to calling this function.
   */
  function render_link($data, $values) {
    //dsm($this->additional_fields);
    //dsm($this->additional_fields);
    if (!empty($this->options['link_to_object']) && !empty($this->additional_fields['PID'])) {
      if ($data !== NULL && $data !== '') {
        $this->options['alter']['make_link'] = TRUE;
        $this->options['alter']['path'] = "fedora/repository/" . $this->get_value($values, 'PID');
//        if (isset($this->aliases['language'])) {
//          $languages = language_list();
//          $language = $this->get_value($values, 'language');
//          if (isset($languages[$language])) {
//            $this->options['alter']['language'] = $languages[$language];
//          }
//          else {
//            unset($this->options['alter']['language']);
//          }
//        }
      }
      else {
        $this->options['alter']['make_link'] = FALSE;
      }
    }
    return $data;
  }
  
  function render($values) {
    $value = $this->get_value($values);
    return $this->render_link($this->sanitize_value($value), $values);
  }
  
}