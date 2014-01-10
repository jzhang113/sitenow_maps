<?php
/**
 * @file
 * Ctools content type plugin file for a pane displaying an individual building.
 */

$plugin = array(
  'single' => TRUE,
  'title' => t('Campus Building'),
  'description' => t('Shows a map of a campus building.'),
  'category' => t('UIowa'),
  'required context' => new ctools_context_required(t('Tokens'),'node'),
  'render callback' => 'individual_building_render',
  'admin info' => 'individual_building_admin_info',
  'edit form' => 'individual_building_edit_form',
  'defaults' => array(
  	'building' => '',
  	'address' => 1
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
  $form['address'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show Address'),
    '#description' => t('Check this box to display the building\'s street address'),
    '#default_value' => $conf['address'],
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
  if($context->data->field_abbreviation){
    $conf['building'] = $context->data->field_abbreviation['und'][0]['value'];
  }
	$info = simplexml_load_file("http://maps.uiowa.edu/api/v1/".$conf['building']."/detail.xml");
	$metadata = '';
	if($conf['address']){
		$metadata .= '<p>Address: '.t($info->building->address).'</p>';
	}
	$block->title = t($info->building->title);
	$block->content = array(
		'#prefix' => '<div class="building_panel_'.$conf['building'].'">',
		'#markup' => '<div id="building_map_'.$conf['building'].'" style="width:100%; height:300px"></div>'.$metadata,
		'#suffix' => '</div>',
	);
	drupal_add_js(array('abbr' => $conf['building']), 'setting');
	drupal_add_js('http://api.tiles.mapbox.com/mapbox.js/v1.5.2/mapbox.js', 'external');
	drupal_add_css('http://api.tiles.mapbox.com/mapbox.js/v1.5.2/mapbox.css', 'external');
	drupal_add_js(drupal_get_path('module', 'maps_panes') . '/js/proj4js-compressed.js');
	drupal_add_js(drupal_get_path('module', 'maps_panes') . '/js/individual_building.js');
	return $block;
}