<?php

function workout_tracker($form, &$form_state){
	global $user;

	$form['#tree'] = TRUE;

	if(!empty($form_state['page_num']) && $form_state['page_num'] == 2){
		return workout_tracker_workout($form, $form_state);
	}

	$form_state['page_num'] = 1;

	$strength_exists = FALSE;
	$conditioning_exists = FALSE;


	$top_strength = db_query('SELECT wid, date FROM {workout_builder_strength} ORDER BY wid DESC LIMIT 7');
	$top_conditioning = db_query('SELECT wid, date FROM {workout_builder_conditioning} ORDER BY wid DESC LIMIT 7');

	if(empty($top_strength) && empty($top_conditioning)){
		drupal_set_message('<pre>' . print_r("There are no specified workouts, please use the Workout Builder to create a workout.", TRUE) . '</pre>');
		return;
	}

	// Take the last 10 workouts from both strength and conditioning
	// and make them one unified array to populate radio buttons
	foreach($top_strength as $ts){
		$top_ten[$ts->wid] = convert_timestamp($ts->date);
	}
	foreach($top_conditioning as $ts){
		// check to see if this timestamp is already in the array
		// If it is, it also has a strength portion so don't add it again
		if(!isset($top_ten[$ts->date])){
			$top_ten[$ts->wid] = convert_timestamp($ts->date);
		}
	}

	krsort($top_ten);

//	drupal_set_message('<pre>' . print_r($top_ten, TRUE) . '</pre>');

	$form['description'] = array(
		'#type' => 'item',
		'#title' => t('Pick a Workout'), 
	);

	$form['workout_choice'] = array(
		'#type' => 'radios',
		'#options' => $top_ten,
		'#title' => t('Choose which workout you performed'),
		'#default_value' => key($top_ten),
	);	

	$form['next'] = array(
		'#type' => 'submit',
		'#value' => 'Track Workout',
		'#submit' => array('workout_tracker_next_submit'),
	);

	return $form;

}

function workout_tracker_next_submit($form, &$form_state){
//	drupal_set_message('<pre>' . print_r($form_state, TRUE) . '</pre>');

	$form_state['page_num'] = 2;
	$form_state['rebuild'] = TRUE;
}

function workout_tracker_workout($form, &$form_state){
	// Build this form based off the workout they just chose
	$wid = $form_state['values']['workout_choice'];

	//drupal_set_message('<pre>' . print_r($wid, TRUE) . '</pre>');

	$strength = db_query('SELECT * FROM {workout_builder_strength} where wid=:wid', array(':wid' => $wid));
	$conditioning = db_query('SELECT * FROM {workout_builder_conditioning} where wid=:wid', array(':wid' => $wid));

	if(!empty($strength)){
		$strength_data = array();	
		// put the rows from the db into an array for looping by strength_id
		foreach($strength as $movement){
			$strength_data[] = $movement;
		}
		drupal_set_message('<pre>' . print_r($strength_data, TRUE) . '</pre>');
	}	


	return $form;
}

function convert_timestamp($timestamp){
	$format = 'D d M Y';
	return date($format, $timestamp);
}

