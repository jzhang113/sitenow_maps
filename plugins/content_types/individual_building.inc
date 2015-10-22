<?php
/**
 * @file
 * Ctools content type plugin file for a pane displaying an individual building.
 */

$plugin = array(
  'single' => TRUE,
  'title' => t('Campus Building'),
  'description' => t('Shows a map of a campus building.'),
  'category' => t('Sitenow'),
  'all contexts' => TRUE,
  'render callback' => 'individual_building_render',
  'admin info' => 'individual_building_admin_info',
  'edit form' => 'individual_building_edit_form',
  'defaults' => array(
    'building' => '',
    'building_override' => '',
    'address' => 1,
    'map_height' => '300',
    'other_buildings' => 0,
    'accessible_entrances' => 0,
    'scrollwheel' => 1,
  ),
);

/**
 * The 'admin info' callback for panel pane.
 */
function individual_building_info($subtype, $conf, $contexts) {
  if (!empty($conf)) {
    $block = new stdClass();
    $block->title = $conf['override_title'] ? $conf['override_title_text'] : '';
    $block->content = t('A map of a University of Iowa building.');
    return $block;
  }
}

/**
 * The 'Edit form' callback for the content type.
 */
function individual_building_edit_form($form, &$form_state) {
  $conf = $form_state['conf'];
  // Create select list for which building to show.
  $form['building'] = array(
    '#type' => 'select',
    '#title' => t('Building'),
    '#description' => t('Select which building to show'),
    '#options' => sitenow_maps_building_list(),
    '#default_value' => $conf['building'],
  );
  // Create select list for which building to show.
  $form['building_set'] = array(
    '#type' => 'fieldset',
    '#title' => t('Building - Advanced Override'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  // Override.
  $form['building_set']['building_override'] = array(
    '#type' => 'textfield',
    '#title' => t('Building'),
    '#description' => t('Provide a field context or manual overide. You may use context keywords.'),
    '#default_value' => $conf['building_override'],
  );
  // Set the height of the map.
  $form['map_height'] = array(
    '#type' => 'textfield',
    '#title' => t('Map Height'),
    '#description' => t('Set the height of the map in pixels.'),
    '#default_value' => $conf['map_height'],
  );
  // Choose whether to show the street address or not.
  $form['address'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show Address'),
    '#description' => t("Check this box to display the building's street address"),
    '#default_value' => $conf['address'],
  );
  // Choose whether to show other buildings.
  $form['other_buildings'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show Other Buildings'),
    '#description' => t('Check this box to show other buildings on the map, centering it on the current building'),
    '#default_value' => $conf['other_buildings'],
  );
  // Choose whether to show other buildings.
  $form['accessible_entrances'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show All Accessible Entrances'),
    '#description' => t('Check this box to show all accessible entrances on campus'),
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
function individual_building_edit_form_submit($form, &$form_state) {
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
function individual_building_render($subtype, &$conf, $panel_args, $context) {
  // Set the building variable from conf.
  $building = $conf['building'];

  if (!empty($conf['building_override'])) {
    // If the building override field is not empty, get any ctools context
    // keyword substitutions.
    $building = ctools_context_keyword_substitute($conf['building_override'], array(), $context);
    if (empty($building)) {
      // If there are no context values, return.
      return;
    };
  }

  // Loads the building data from the maps site. More building data inception!
  $info = simplexml_load_file("http://maps.uiowa.edu/api/v1/" . $building . "/detail.xml");

  $metadata = '';
  if ($conf['address']) {
    $metadata .= '<p>Address: ' . t('@address', array('@address' => $info->building->address)) . '</p>';
  }
  $block = new stdClass();
  $block->title = t('@title', array('@title' => $info->building->title));
  $block->content = array(
    '#prefix' => '<div class="building_panel_' . $building . '">',
    '#markup' => '<div id="building_map_' . $building . '" style="height:' . (string) $conf['map_height'] . 'px;width:100%;"></div>' . $metadata,
    '#suffix' => '</div>',
  );

  proj4js_load_definition("EPSG:3418", "+title=Iowa South (ft US) +proj=lcc +lat_1=41.78333333333333 +lat_2=40.61666666666667 +lat_0=40 +lon_0=-93.5 +x_0=500000.00001016 +y_0=0 +ellps=GRS80 +datum=NAD83 +to_meter=0.3048006096012192 +no_defs  ");

  drupal_add_js(array('abbr' => $building), 'setting');
  drupal_add_js(array('latitude' => $info->building->latitude), 'setting');
  drupal_add_js(array('longitude' => $info->building->longitude), 'setting');
  drupal_add_js(array('other_buildings' => $conf['other_buildings']), 'setting');
  if ($conf['other_buildings']) {
    drupal_add_js(array('all_buildings' => sitenow_maps_building_list()), 'setting');
  }
  drupal_add_js(array('accessible_entrances' => $conf['accessible_entrances']), 'setting');

  // Pass in the scrollwheel zoom settings.
  drupal_add_js(array('scrollwheel' => !isset($conf['scrollwheel']) ? 1 : $conf['scrollwheel']), 'setting');

  drupal_add_js('https://maps.googleapis.com/maps/api/js?v=3.exp&key=AIzaSyACV3y6WAUNmC4zok1_VwoKx60qpyZE9sU', 'external');
  drupal_add_js(drupal_get_path('module', 'sitenow_maps') . '/js/individual_building.js');
  return $block;
}
