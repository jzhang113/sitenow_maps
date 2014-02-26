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
  'required context' => new ctools_context_required(t('Tokens'),'node'),
  'render callback' => 'individual_building_render',
  'admin info' => 'individual_building_admin_info',
  'edit form' => 'individual_building_edit_form',
  'defaults' => array(
  	'building' => '',
  	'address' => 1,
    'map_height' => '300',
    'other_buildings' => 0,
    'accessible_entrances' => 0
  )
);

/**
 * The 'admin info' callback for panel pane.
 */
function individual_building_info($subtype, $conf, $contexts) {
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
function individual_building_edit_form($form, &$form_state) {
  $conf = $form_state['conf'];
  //Create select list for which building to show
  $form['building'] = array(
    '#type' => 'select',
    '#title' => t('Building'),
    '#description' => t('Select which building to show'),
    '#options' => get_building_list(),
    '#default_value' => $conf['building'],
  );
  //Set the height of the map
  $form['map_height'] = array(
    '#type' => 'textfield',
    '#title' => t('Map Height'),
    '#description' => t('Set the height of the map in pixels.'),
    '#default_value' => $conf['map_height'],
  );
  //Choose whether to show the street address or not
  $form['address'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show Address'),
    '#description' => t('Check this box to display the building\'s street address'),
    '#default_value' => $conf['address'],
  );
  //Choose whether to show other buildings
  $form['other_buildings'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show Other Buildings'),
    '#description' => t('Check this box to show other buildings on the map, centering it on the current building'),
    '#default_value' => $conf['other_buildings'],
  );
  //Choose whether to show other buildings
  $form['accessible_entrances'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show All Accessible Entrances'),
    '#description' => t('Check this box to show all accessible entrances on campus'),
    '#default_value' => $conf['accessible_entrances'],
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
  //Checks to see if a token context exists and if so, if a field called field_abbreviation exists. If so, it defines the building code to that allowing for dynamic setting of the building map
  if (isset($context->data->field_abbreviation)) {
    $conf['building'] = $context->data->field_abbreviation[LANGUAGE_NONE][0]['value'];
  }

  // Loads the building data from the maps site. More building data inception!
	$info = simplexml_load_file("http://maps.uiowa.edu/api/v1/".$conf['building']."/detail.xml");

  $metadata = '';
	if($conf['address']){
		$metadata .= '<p>Address: '.t($info->building->address).'</p>';
	}
	$block->title = t($info->building->title);
	$block->content = array(
		'#prefix' => '<div class="building_panel_'.$conf['building'].'">',
		'#markup' => '<div id="building_map_'.$conf['building'].'" style="height:'.(string)$conf['map_height'].'px;width:100%;"></div>'.$metadata,
		'#suffix' => '</div>',
	);

	drupal_add_js(array('abbr' => $conf['building']), 'setting');
  drupal_add_js(array('latitude' => $info->building->latitude), 'setting');
  drupal_add_js(array('longitude' => $info->building->longitude), 'setting');
  drupal_add_js(array('other_buildings' => $conf['other_buildings']), 'setting');
  //if($conf['other_buildings']) drupal_add_js(array('all_buildings' => sitenow_maps_building_list()), 'setting');
  drupal_add_js(array('accessible_entrances' => $conf['accessible_entrances']), 'setting');
	drupal_add_js('http://api.tiles.mapbox.com/mapbox.js/v1.6.1/mapbox.js', 'external');
	drupal_add_css('http://api.tiles.mapbox.com/mapbox.js/v1.6.1/mapbox.css', 'external');
	drupal_add_js(drupal_get_path('module', 'sitenow_maps') . '/js/proj4js-compressed.js');
	drupal_add_js(drupal_get_path('module', 'sitenow_maps') . '/js/individual_building.js');
	return $block;
}
