<?php

// print $view_mode;
$classes = array('m8fieldset');
$classes[] = 'm8fieldset-'. $m8fieldset->type;
$classes[] = $view_mode;


print '<div id="m8fieldset-'. $m8fieldset->id .'" class="'. join(' ', $classes) .'">';
	// на странице сущности выводим информауию о целе
	if (arg(0) == 'm8fieldset'){
		$target = m8fieldset_target_summary($m8fieldset);
		print render($target);
	}

	// содержимое
	print render($m8fieldset->content);

	if ($view_mode == 'full'){
		// ссылки
		$actions = array(
		    '#theme' => 'links', 
		    '#attributes' => array('class' => 'inline m8fieldset-links'),
		    //'#heading' => 'Fieldset actions',
		);
			$uri = m8fieldset_uri($m8fieldset);
			// страница просмотра
			$actions['#links']['view'] = array('title' => t('view'), 'href' => $uri['path']);
			// редактирование
			if (m8fieldset_access_calback('update', $m8fieldset)){
				$actions['#links']['edit'] = array('title' => t('edit'), 'href' => $uri['path'] .'/edit', 'query' => drupal_get_destination());
			}
			// удаление
			if (element_children($m8fieldset->content)){
				//$actions['#links']['delete'] = array('title' => 'delete', 'href' => $uri['path'] .'/delete');
			}
		print render($actions);
	}

print '</div>';