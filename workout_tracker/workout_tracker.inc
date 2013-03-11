<?php

function workout_tracker($form, &$form_state){
	global $user;

	$strength_exists = FALSE;
	$conditioning_exists = FALSE;


	$top_strength = db_query('SELECT date FROM {workout_builder_strength} ORDER BY date LIMIT 7');
	$top_conditioning = db_query('SELECT date FROM {workout_builder_conditioning} ORDER BY date LIMIT 7');

	if(empty($top_strength) && empty($top_conditioning)){
		drupal_set_message('<pre>' . print_r("There are no specified workouts, please use the Workout Builder to create a workout.", TRUE) . '</pre>');
		return;
	}

	// Take the last 10 workouts from both strength and conditioning
	// and make them one unified array to populate radio buttons
	foreach($top_strength as $ts){
		$top_ten[$ts->date] = convert_timestamp($ts->date);
	}
	foreach($top_conditioning as $ts){
		// check to see if this timestamp is already in the array
		// If it is, it also has a strength portion so don't add it again
		if(!isset($top_ten[$ts->date])){
			$top_ten[$ts->date] = convert_timestamp($ts->date);
		}
	}

	drupal_set_message('<pre>' . print_r($top_ten, TRUE) . '</pre>');

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

	return $form;

}

function convert_timestamp($timestamp){
	$format = 'D d M Y';
	return date($format, $timestamp);
}

