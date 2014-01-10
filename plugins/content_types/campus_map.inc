<?php
/**
 * @file
 * Ctools content type plugin file for a pane displaying an individual building.
 */

$plugin = array(
  'single' => TRUE,
  'title' => t('Campus Map'),
  'description' => t('Shows a map of all campus buildings.'),
  'category' => t('UIowa'),
  'render callback' => 'campus_map_render',
  'admin info' => 'campus_map_admin_info',
  'edit form' => 'campus_map_edit_form',
  'defaults' => array(
  	'building' => '',
    'map_height' => '300'
  )
);

/**
 * The 'admin info' callback for panel pane.
 */
function campus_map_info($subtype, $conf, $contexts) {
  if (!empty($conf)) {
    $block = new stdClass();
    $block->title = $conf['override_title'] ? $conf['override_title_text'] : '';
    $block->content = t('The University of Iowa nameplate.');
    return $block;
  }
}

/**
 * The 'Edit form' callback for the content type.
 */
function campus_map_edit_form($form, &$form_state) {
  $conf = $form_state['conf'];
  //Create select list for which building to show
  $form['map_height'] = array(
    '#type' => 'textfield',
    '#title' => t('Map Height'),
    '#description' => t('Set the height of the map in pixels.'),
    '#default_value' => $conf['map_height'],
  );
  return $form;
}

/**
 * The submit form stores the data in $conf.
 */
function campus_map_edit_form_submit($form, &$form_state) {
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
function campus_map_render($subtype, $conf, $panel_args, $context = NULL) {
	$block->title = t('Campus Map');
	$block->content = array(
		'#prefix' => '<div id="campus_map_panel">',
		'#markup' => '<div id="campus_map" style="height:'.(string)$conf['map_height'].'px;width:100%; ></div>',
		'#suffix' => '</div>',
	);
	drupal_add_js(array('buildings' => get_building_list()), 'setting');
	drupal_add_js('http://api.tiles.mapbox.com/mapbox.js/v1.6.0/mapbox.js', 'external');
	drupal_add_css('http://api.tiles.mapbox.com/mapbox.js/v1.6.0/mapbox.css', 'external');
	drupal_add_js(drupal_get_path('module', 'maps_panes') . '/js/proj4js-compressed.js');
	drupal_add_js(drupal_get_path('module', 'maps_panes') . '/js/campus_map.js');
	return $block;
}