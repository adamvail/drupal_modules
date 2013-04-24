<?php

function workout_pr($form, &$form_state){
	//return workout_pr_create_table();
	return workout_pr_create_datatable();
}

function workout_pr_create_table(){
	global $user;

	$prs = db_query("SELECT * from {workout_pr} where athlete_uid=:aid", array(':aid' => $user->uid))->fetchAll();

	$table = array(
		'#theme' => 'table',
		'#header' => array('Lift', 'Weight'),
		'#rows' => array(),
	);

	foreach($prs as $pr){
		$units = '';
		if($pr->units == 0){
			$units = 'lbs';
		}
		elseif($pr->units == 1) {
			$units = 'kg';
		}

		$table['#rows'][] = array(pretty_print_movement($pr->movement), $pr->pr . ' ' . $units);
	}

//	drupal_set_message("<pr>" . print_r($table, TRUE) . "</pr>");

	return $table;
}

function pretty_print_movement($movement){
	$movement_pretty = implode(' ', array_map('ucfirst', explode(' ', $movement)));
	$movement_pretty = implode('-', array_map('ucfirst', explode('-', $movement_pretty)));
	$movement_pretty = implode('[', array_map('ucfirst', explode('[', $movement_pretty)));
	$movement_pretty = implode(']', array_map('ucfirst', explode(']', $movement_pretty)));
	$movement_pretty = implode('+', array_map('ucfirst', explode('+', $movement_pretty)));
	return $movement_pretty;
}

function workout_pr_create_datatable(){
	global $user;

	$attributes['datatable_options'] = array(
		'bFilter' => TRUE,
		'bInfo' => TRUE,
	);

	$header = array(
		array(
			'data' => t('Lift'),
			'datatable_options' => array(
				'bSortable' => TRUE,
				'bSearchable' => TRUE,
			),
		),
		array(
			'data' => t('Weight'),
			'datatable_options' => array(
				'bSortable' => TRUE,
				'bSearchable' => TRUE,
			),
		),
	);

	$prs = db_query("SELECT * from {workout_pr} where athlete_uid=:aid", array(':aid' => $user->uid))->fetchAll();
	
	$rows = array();
	foreach($prs as $pr){
		$units = '';
		if($pr->units == 0){
			$units = 'lbs';
		}
		elseif($pr->units == 1) {
			$units = 'kg';
		}

		$rows[] = array(pretty_print_movement($pr->movement), $pr->pr . ' ' . $units);
	}

	$build['datatable'] = array(
		'#theme' => 'datatable',
		'#header' => $header,
		'#rows' => $rows,
		'#attributes' => $attributes,
	);

	return $build;
//	return theme($variables);
}
