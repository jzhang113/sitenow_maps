<?php
/**
 * @file
 * Ctools content type plugin file for a pane displaying an individual building.
 */

$plugin = array(
  'single' => TRUE,
  'title' => t('Map Point'),
  'description' => t('Shows a map of a latitude/longitude point using the university map tiles.'),
  'category' => t('Sitenow'),
  'all contexts' => TRUE,
  'render callback' => 'pin_render',
  'admin info' => 'pin_admin_info',
  'edit form' => 'pin_edit_form',
  'defaults' => array(
    'latitude' => '',
    'longitude' => '',
    'map_height' => '300',
    'buildings' => 0,
    'accessible_entrances' => 0,
    'popup_text' => '',
    'scrollwheel' => 1,
  ),
);

/**
 * The 'admin info' callback for panel pane.
 */
function pin_info($subtype, $conf, $contexts) {
  if (!empty($conf)) {
    $block = new stdClass();
    $block->title = $conf['override_title'] ? $conf['override_title_text'] : '';
    $block->content = t('A map of a point on the University of Iowa map.');
    return $block;
  }
}

/**
 * The 'Edit form' callback for the content type.
 */
function pin_edit_form($form, &$form_state) {
  $conf = $form_state['conf'];

  // Set the height of the map.
  $form['map_height'] = array(
    '#type' => 'textfield',
    '#title' => t('Map Height'),
    '#description' => t('Set the height of the map in pixels.'),
    '#default_value' => $conf['map_height'],
  );

  $form['latitude'] = array(
    '#type' => 'textfield',
    '#title' => t('Latitude'),
    '#description' => t('Set the point latitidude.'),
    '#default_value' => $conf['latitude'],
  );

  $form['longitude'] = array(
    '#type' => 'textfield',
    '#title' => t('Longitude'),
    '#description' => t('Set the point longitude.'),
    '#default_value' => $conf['longitude'],
  );

  $form['popup_text'] = array(
    '#type' => 'textfield',
    '#title' => t('Popup Text'),
    '#description' => t('Text available when the pin is clicked or tapped.'),
    '#default_value' => $conf['popup_text'],
  );

  // Choose whether to show other buildings.
  $form['buildings'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show Buildings'),
    '#description' => t('Check this box to show university buildings on the map.'),
    '#default_value' => $conf['buildings'],
  );
  // Choose whether to show other buildings.
  $form['accessible_entrances'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show All Accessible Entrances'),
    '#description' => t('Check this box to show all accessible entrances on campus.'),
    '#default_value' => $conf['accessible_entrances'],
  );
  // Choose whether to allow scroll wheel zoom.
  $form['scrollwheel'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow Scrollwheel Zoom'),
    '#description' => t('Check this box to allow the mouse scrollwheel to zoom the map'),
    '#default_value' => $conf['scrollwheel'],
  );

  return $form;
}

/**
 * The submit form stores the data in $conf.
 */
function pin_edit_form_submit($form, &$form_state) {
  foreach (array_keys($form_state['plugin']['defaults']) as $key) {
    if (isset($form_state['values'][$key])) {
      $form_state['conf'][$key] = $form_state['values'][$key];
    }
  }
}

/**
 * Run-time rendering of the body of the block (content type).
 *
 * See ctools_plugin_examples for more advanced info.
 */
function pin_render($subtype, &$conf, $panel_args, $context) {
  // Search the values of the latitude and longitude fields for any ctools
  // context keywords to substitute.
  $latitude = check_plain(ctools_context_keyword_substitute($conf['latitude'], array(), $context));
  $longitude = check_plain(ctools_context_keyword_substitute($conf['longitude'], array(), $context));
  $popup_text = check_plain(ctools_context_keyword_substitute($conf['popup_text'], array(), $context));

  if (empty($latitude) || empty($longitude)) {
    // If either the latitude or longitude values are empty, return.
    return;
  }

  $block = new stdClass();

  $block->content = array(
    '#prefix' => '<div class="building_panel_pin">',
    '#markup' => '<div id="pin_map" style="height:' . (string) check_plain($conf['map_height']) . 'px;width:100%;"></div>',
    '#suffix' => '</div>',
  );

  proj4js_load_definition("EPSG:3418", "+title=Iowa South (ft US) +proj=lcc +lat_1=41.78333333333333 +lat_2=40.61666666666667 +lat_0=40 +lon_0=-93.5 +x_0=500000.00001016 +y_0=0 +ellps=GRS80 +datum=NAD83 +to_meter=0.3048006096012192 +no_defs  ");
  drupal_add_js(array('sitenowMaps' => array('latitude' => $latitude)), 'setting');
  drupal_add_js(array('sitenowMaps' => array('longitude' => $longitude)), 'setting');
  if ($conf['popup_text']) {
    drupal_add_js(array('sitenowMaps' => array('popup_text' => check_plain($conf['popup_text']))), 'setting');
  }

  drupal_add_js(array('buildings' => $conf['buildings']), 'setting');
  if ($conf['buildings']) {
    drupal_add_js(array('all_buildings' => sitenow_maps_building_list()), 'setting');
  }
  drupal_add_js(array('accessible_entrances' => $conf['accessible_entrances']), 'setting');

  // Pass in the scrollwheel zoom settings.
  drupal_add_js(array('scrollwheel' => !isset($conf['scrollwheel']) ? 1 : $conf['scrollwheel']), 'setting');

  drupal_add_js('https://maps.googleapis.com/maps/api/js?v=3.exp&key=AIzaSyACV3y6WAUNmC4zok1_VwoKx60qpyZE9sU', 'external');
  drupal_add_js(drupal_get_path('module', 'sitenow_maps') . '/js/pin.js');
  return $block;
}
