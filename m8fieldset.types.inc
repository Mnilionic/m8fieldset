<?php

function m8fieldset_types_overview(){
	$field_ui = module_exists('field_ui');
	$header = array(t('Name'), array('data' => t('Operations'), 'colspan' => $field_ui ? '4' : '2'));
	$rows = array();

	foreach (m8fieldset_get_types() as $key => $type) {
		$row = array();
		$type_url_str = str_replace('_', '-', $key);
		$row[] = theme('m8fieldset_admin_overview', array('type' => $type));
		$row[] = array('data' => l(t('edit'), 'admin/structure/m8fieldset/manage/' . $type_url_str));
		if ($field_ui) {
        	// Manage fields.
        	$row[] = array('data' => l(t('manage fields'), 'admin/structure/m8fieldset/manage/' . $type_url_str . '/fields'));
        	// Display fields.
        	$row[] = array('data' => l(t('manage display'), 'admin/structure/m8fieldset/manage/' . $type_url_str . '/display'));
      	}
      	$row[] = array('data' => l(t('delete'), 'admin/structure/m8fieldset/manage/' . $type_url_str . '/delete'));
      	$rows[] = $row;
    }

	$build['node_table'] = array(
		'#theme' => 'table',
		'#header' => $header,
		'#rows' => $rows,
		'#empty' => t('No fieldset types available. <a href="@link">Add fieldset type</a>.', array('@link' => url('admin/structure/m8fieldset/add'))),
	);

	return $build;
}


function m8fieldset_type_set_defaults($info = array()){
	$info = (array) $info;
	$new_type = $info + array(
		'type' => '',
		'label' => '',
		'description' => '',
		'help' => '',
		'old_type' => '',
	);
	return (object) $new_type;
}


function m8fieldset_type_form($form, &$form_state, $type = NULL){

	if (!$type){
		$type = m8fieldset_type_set_defaults();
        $new = TRUE;
	}

	$form['#m8fieldset_type'] = $type;

	$form['name'] = array(
	    '#title' => t('Name'),
	    '#type' => 'textfield',
	    '#default_value' => $type->label,
	    '#description' => t('The human-readable name of this fieldset type. It is recommended that this name begin with a capital letter and contain only letters, numbers, and spaces. This name must be unique.'),
	    '#required' => TRUE,
	    '#size' => 30,
    );

	$form['type'] = array(
    	'#type' => 'machine_name',
    	'#default_value' => $type->type,
    	'#maxlength' => 32,
    	'#machine_name' => array(
      		'exists' => 'm8fieldset_type_load',
    	),
    	'#description' => t('A unique machine-readable name for this fieldset type. It must only contain lowercase letters, numbers, and underscores. This name will be used to identify type of fieldset.'),
  	);
  	// если захотим переименовать бандл, сюда сохраним старое название
	$form['old_type'] = array(
    	'#type' => 'value',
    	'#value' => $type->type,
  	);


  	$form['description'] = array(
    	'#title' => t('Description'),
    	'#type' => 'textarea',
    	'#default_value' => $type->description,
    	'#description' => t('Describe this fieldset type. The text will be displayed on the page of available types.'),
  	);
  	$form['help'] = array(
    	'#title' => t('Explanation or submission guidelines'),
    	'#type' => 'textarea',
    	'#default_value' => $type->help,
    	'#description' => t('This text will be displayed at the top of the page when creating or editing content of this fieldset type.'),
  	);
  	//
	$form['submit'] = array(
		'#type' => 'submit',
		'#value'  => t('Save'),
	);
	$form['delete'] = array(
		'#type' => 'submit',
		'#value'  => t('Delete'),
		//'#limit_validation_errors' => array(),
        '#access' => empty($new),
	);
	return $form;
}


function m8fieldset_type_form_submit($form, &$form_state) {
	$v = &$form_state['values'];
	$op = $form_state['triggering_element']['#parents'][0];
	if ($op == 'delete') {
		$form_state['redirect'] = 'admin/structure/m8fieldset/manage/' . str_replace('_', '-', $v['old_type']) . '/delete';
		return;
	}

	$type = m8fieldset_type_set_defaults();

	$type->type = $v['type'];
	$type->label = trim($v['name']);
	$type->description = $v['description'];
	$type->help = $v['help'];
    $type->old_type = isset($v['old_type']) ? $v['old_type'] : $type->type;

    $status = m8fieldset_type_save($type);
    menu_rebuild();
    $t_args = array('%label' => $type->label);

	if ($status == SAVED_UPDATED) {
        drupal_set_message(t('The fieldset type %label has been updated.', $t_args));
	}
	elseif ($status == SAVED_NEW) {
    	drupal_set_message(t('The fieldset type %label has been added.', $t_args));
    	watchdog('m8fieldset', 'Added fieldset type %label.', $t_args, WATCHDOG_NOTICE, l(t('view'), 'admin/structure/m8fieldset'));
	}

	$form_state['redirect'] = 'admin/structure/m8fieldset';
	return;
}


// сохранить тип
function m8fieldset_type_save($info){

  $existing_type = !empty($info->old_type) ? $info->old_type : $info->type;
  $is_existing = (bool) db_query_range('SELECT 1 FROM {m8fieldset_type} WHERE type = :type', 0, 1, array(':type' => $existing_type))->fetchField();
  $type = m8fieldset_type_set_defaults($info);

  $fields = array(
    'type' => (string) $type->type,
    'label' => (string) $type->label,
    'description' => (string) $type->description,
    'help' => (string) $type->help,
  );

  	if ($is_existing) {
    	db_update('m8fieldset_type')
      		->fields($fields)
      		->condition('type', $existing_type)
      		->execute();

    	if (!empty($type->old_type) && $type->old_type != $type->type) {
      		field_attach_rename_bundle('m8fieldset', $type->old_type, $type->type);
    	}
    	$status = SAVED_UPDATED;
    	//
    	$update_count = m8fieldset_type_update_type($info->old_type, $info->type);
	    if ($update_count) {
	      drupal_set_message(format_plural($update_count, 'Changed the fieldset type of 1 set from %old-type to %type.', 'Changed the fieldset type of @count sets from %old-type to %type.', array('%old-type' => $info->old_type, '%type' => $info->type)));
	    }
  	}
  	else {
    	db_insert('m8fieldset_type')
      		->fields($fields)
      		->execute();

    	field_attach_create_bundle('m8fieldset', $type->type);
    	$status = SAVED_NEW;
  	}
  	return $status;
}

// поменять старые типы на новые
function m8fieldset_type_update_type($old_type, $new_type){
    // получаем список полей
    $q = db_select('field_config', 'fc', array('fetch' => PDO::FETCH_ASSOC));
    $q->join('field_config_instance', 'fci', 'fc.id = fci.field_id');
    $q->fields('fci', array('field_name' ,'entity_type', 'bundle'));
    $q->condition('fc.type', 'm8fieldset_field');
    $q->condition('fci.deleted', '0');
    $result = $q->execute();
    // обновляем настройки полей
    foreach ($result as $record) {
        $instance_info = field_info_instance($record['entity_type'], $record['field_name'], $record['bundle']);
        if (!empty($instance_info['settings']['list'][$old_type])){
            unset($instance_info['settings']['list'][$old_type]);
            $instance_info['settings']['list'][$new_type] = 1;
        }
        field_update_instance($instance_info);
    }
    // обновляем типы у филдсетов
    return db_update('m8fieldset')
        ->fields(array('type' => $new_type))
        ->condition('type', $old_type)
        ->execute();
}

// удалить тип 
function m8fieldset_type_delete($type){
  	db_delete('m8fieldset_type')
    	->condition('type', $type)
    	->execute();
  	field_attach_delete_bundle('m8fieldset', $type);
}


function m8fieldset_type_delete_confirm($form, &$form_state, $type){
  	$form['type'] = array('#type' => 'value', '#value' => $type->type);
  	$form['label'] = array('#type' => 'value', '#value' => $type->label);
 
  	$message = t('Are you sure you want to delete the fieldset type %type?', array('%type' => $type->label));
  	$caption = '';

  	$counts = db_query("SELECT COUNT(*) FROM {m8fieldset} WHERE type = :type", array(':type' => $type->type))->fetchField();

  	if ($counts) {
  		// блокируем удаление пока
  		$form['msg'] = array(
  			'#type' => 'item',
  			'#markup' => t('You have a content with this fieldset type. You cannot delete it now.'),
  		);
  		return $form;
  		//-------------------------------
    	$caption .= '<p>' . format_plural($counts, '%type is used by 1 piece of content on your site. If you remove this content type, you will not be able to edit the %type content and it may not display correctly.', '%type is used by @count pieces of content on your site. If you remove %type, you will not be able to edit the %type content and it may not display correctly.', array('%type' => $type->label)) . '</p>';
  	}

  	$caption .= '<p>' . t('This action cannot be undone.') . '</p>';

  	return confirm_form($form, $message, 'admin/structure/m8fieldset', $caption, t('Delete'));
}


function m8fieldset_type_delete_confirm_submit($form, &$form_state) {
	m8fieldset_type_delete($form_state['values']['type']);

	$t_args = array('%name' => $form_state['values']['label']);
	drupal_set_message(t('The fieldset type %name has been deleted.', $t_args));
	watchdog('m8fieldset', 'Deleted fieldset type %name.', $t_args, WATCHDOG_NOTICE);

	menu_rebuild();
    entity_get_controller('m8fieldset')->resetCache();

	$form_state['redirect'] = 'admin/structure/m8fieldset';
	return;
}



function theme_m8fieldset_admin_overview($variables) {
	$type = $variables['type'];

	$output = check_plain($type->label);
	$output .= ' <small>' . t('(Machine name: @type)', array('@type' => $type->type)) . '</small>';
	$output .= '<div class="description">' . filter_xss_admin($type->description) . '</div>';
	return $output;
}