<?php

/**
 * @file
 * Field hooks to implement a date field.
 */

/**
 * Implements hook_field_info().
 */
function sitenow_maps_field_info() {
  return array(
    'sitenow_map' => array(
      'label' => t('Sitenow Map'),
      'description' => t('A SiteNow Map Field'),
      'default_widget' => 'sitenow_map_widget_building_select',
      'default_formatter' => 'sitenow_map_formatter_abbr',
    ),
  );
}

/**
 * Implements hook_field_widget_info().
 */
function sitenow_maps_field_widget_info() {
  return array(
    'sitenow_map_widget_building_select' => array(
      'label' => t('Building Select'),
      'field types' => array('sitenow_map'),
      'behaviors' => array(
        'multiple values' => FIELD_BEHAVIOR_DEFAULT,
        'default value' => FIELD_BEHAVIOR_DEFAULT,
      ),
    ),
  );
}

/**
 * Implements hook_field_widget_form().
 */
function sitenow_maps_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
  switch ($instance['widget']['type']) {
    case 'sitenow_map_widget_building_select':
      $buildings = sitenow_maps_building_list();
      $element['sitenow_map']['value'] = array(
        '#type' => 'select',
        '#title' => t('Building Location'),
        '#maxlength' => 255,
        '#options' => $buildings,
        '#empty_value' => '',
        '#required' => $element['#required'],
        '#default_value' => isset($items[$delta]['value']) ? $items[$delta]['value'] : $instance['default_value'][0]['sitenow_map']['value'],
      );
      break;
  }
  return $element;
}

/**
 * Implements hook_field_validate().
 */
function sitenow_maps_field_validate($entity_type, $entity, $field, $instance, $langcode, $items, &$errors) {
  if ($field['type'] == 'sitenow_map') {
    foreach ($items as $delta => $item) {
      // If field is not considered empty, validate its data.
      if (!sitenow_maps_field_is_empty($item, $field)) {
      }
    }
  }
}

/**
 * Implements hook_field_presave().
 */
function sitenow_maps_field_presave($entity_type, $entity, $field, $instance, $langcode, &$items) {
  if (!empty($items) && $field['type'] == 'sitenow_map' && array_key_exists('sitenow_map', $items[0])) {
    foreach ($items as $delta => $item) {
      $items[$delta]['value'] = $item['sitenow_map']['value'];
    }
  }
}

/**
 * Implements hook_field_formatter_info().
 */
function sitenow_maps_field_formatter_info() {
  return array(
    'sitenow_map_formatter_abbr' => array(
      'label' => t('Building Abbreviation'),
      'field types' => array('sitenow_map'),
      'settings' => array(
        'link' => 'no',
      ),
    ),
    'sitenow_map_formatter_name' => array(
      'label' => t('Building Name'),
      'field types' => array('sitenow_map'),
      'settings' => array(
        'link' => 'no',
      ),
    ),
  );
}

/**
 * Implements hook_field_formatter_settings_form().
 */
function sitenow_maps_field_formatter_settings_form($field, $instance, $view_mode, $form, &$form_state) {

  $display = $instance['display'][$view_mode];
  $formatter = $display['type'];

  switch ($formatter) {
    case 'sitenow_map_formatter_abbr':
    case 'sitenow_map_formatter_name':
      // Get formatter settings.
      $settings = $display['settings'];
      // Initialize the element variable.
      $element = array();
      // Link to maps.uiowa.edu option.
      $element['link'] = array(
        '#title' => t('Link to maps.uiowa.edu'),
        '#type' => 'select',
        '#default_value' => $settings['link'],
        '#options' => array(
          'no' => t('No'),
          'yes' => t('Yes'),
        ),
      );

      break;

  }
  return $element;
}

/**
 * Implements hook_field_formatter_settings_summary().
 */
function sitenow_maps_field_formatter_settings_summary($field, $instance, $view_mode) {
  $display = $instance['display'][$view_mode];
  $formatter = $display['type'];

  switch ($formatter) {
    case 'sitenow_map_formatter_abbr':
    case 'sitenow_map_formatter_name':
      $settings = $display['settings'];
      $summary = t('Link to maps.uiowa.edu: @link', array(
        '@link' => $settings['link'],
      ));
      break;

  }
  return $summary;
}

/**
 * Implements hook_field_formatter_view().
 */
function sitenow_maps_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = array();
  $settings = $display['settings'];

  switch ($display['type']) {
    // Building Abbreviation.
    case 'sitenow_map_formatter_abbr':
      foreach ($items as $delta => $item) {
        $item_output = '';
        foreach ($item as $column => $value) {
          if ($column == 'value') {
            // If link setting, output link, else plain text.
            $item_output .= ($settings['link'] == 'yes') ? '<a href="http://maps.uiowa.edu/' . $value . '">' . $value . '</a>' : $value;
          }
        }
        $element[$delta]['sitenow_map_row']['content'] = array(
          '#markup' => $item_output,
        );
      }
      break;

    // Building Name.
    case 'sitenow_map_formatter_name':
      $link = $settings['link'];
      $buildings = sitenow_maps_building_list();
      foreach ($items as $delta => $item) {
        $item_output = '';
        foreach ($item as $column => $value) {
          if ($column == 'value') {
            if (isset($buildings[$value])) {
              // If link setting, output link, else plain text.
              $item_output .= ($settings['link'] == 'yes') ? '<a href="http://maps.uiowa.edu/' . $value . '">' . $buildings[$value] . '</a>' : $buildings[$value];
            }
          }
        }
        $element[$delta]['sitenow_map_row']['content'] = array(
          '#markup' => $item_output,
        );
      }
      break;

  }
  return $element;
}

/**
 * Implements hook_field_is_empty().
 */
function sitenow_maps_field_is_empty($item, $field) {
  if ($field['type'] == 'sitenow_map') {
    return FALSE;
  }
}
