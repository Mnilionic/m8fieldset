<?php

function m8fieldset_views_data(){
	$data['m8fieldset']['table']['group'] = t('M8 fieldset');	

	// таблица
	$data['m8fieldset']['table']['base'] = array(
	    'field' => 'id', // This is the identifier field for the view.
	    'title' => 'M8 fieldsets',
	    'help' => '',
	    'weight' => -10,
  	);

	$data['m8fieldset']['table']['entity type'] = 'm8fieldset';

	 // связь
	$data['m8fieldset']['table']['join'] = array(

  	);

	// поля 
	// id
	$data['m8fieldset']['id'] = array(
		'title' => 'Fieldset id',
		'help' => 'Uniq identifier',
		'field' => array(
			'handler' => 'views_handler_field_numeric',
			'click sortable' => TRUE,
		),
	);

	// bundle
	$data['m8fieldset']['type'] = array(
		'title' => 'Fieldset type',
		'help' => 'Entity bundle',
		'field' => array(
			'field_name' => 'type',
		  	'handler' => 'views_handler_field_machine_name',
		  	'click sortable' => TRUE,
		),
		'filter' => array(
		  	'handler' => 'views_handler_filter_string',
		),
		'sort' => array(
		  	'handler' => 'views_handler_sort',
		),
	);
	//
	$data['m8fieldset']['target_id'] = array(
		'title' => 'Target id',
		'help' => 'Entity id',
		'field' => array(
			'field_name' => 'target_id',
		  	'handler' => 'views_handler_field_numeric',
		  	'click sortable' => TRUE,
		),
		'filter' => array(
		  	'handler' => 'views_handler_filter_numeric',
		),
		'sort' => array(
		  	'handler' => 'views_handler_sort',
		),
	);
	//
	$data['m8fieldset']['target_type'] = array(
		'title' => 'Target type',
		'help' => 'Entity type',
		'field' => array(
			'field_name' => 'target_type',
		  	'handler' => 'views_handler_field_machine_name',
		  	'click sortable' => TRUE,
		),
		'filter' => array(
		  	'handler' => 'views_handler_filter_string',
		),
		'sort' => array(
		  	'handler' => 'views_handler_sort',
		),
	);
	//
	$data['m8fieldset']['target_field'] = array(
		'title' => 'Target field',
		'help' => 'Field instance id',
		'field' => array(
			'field_name' => 'target_field',
		  	'handler' => 'views_handler_field_numeric',
		  	'click sortable' => TRUE,
		),
		'filter' => array(
		  	'handler' => 'views_handler_filter_numeric',
		),
		'sort' => array(
		  	'handler' => 'views_handler_sort',
		),
	);

	return $data;
}


function m8fieldset_field_views_data($field){
	$data = field_views_field_default_views_data($field);

	foreach ($data as $table_name => $table_data) {

		$field_name = $field['field_name'] .'_id';
		$data[$table_name][$field_name]['relationship'] = array(
	        'handler' => 'views_handler_relationship',
	        'base' => 'm8fieldset',
	        'base field' => 'id',
	        'label' => 'Referenced M8fieldset',
	        'group' => 'M8 Fieldset',
	        'title' => 'M8fieldset Entity',
	        'help' => 'A bridge to the m8fieldset entity via !field_name.',
	      );
	}

	return $data;
}