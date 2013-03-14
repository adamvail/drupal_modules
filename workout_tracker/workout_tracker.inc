<?php

function workout_tracker($form, &$form_state){
	global $user;

	$form['#tree'] = TRUE;

	if(isset($_GET['wid'])){
		$form_state['values']['workout_choice'] = $_GET['wid'];
		return workout_tracker_workout($form, $form_state);
	}

	if(!empty($form_state['page_num']) && $form_state['page_num'] == 2){
		return workout_tracker_workout($form, $form_state);
	}

	$form_state['page_num'] = 1;

	$strength_exists = FALSE;
	$conditioning_exists = FALSE;


	$top_strength = db_query('SELECT wid, date FROM {workout_builder_strength} ORDER BY wid DESC LIMIT 7');
	$top_conditioning = db_query('SELECT wid, date FROM {workout_builder_conditioning} ORDER BY wid DESC LIMIT 7');

	if(empty($top_strength) && empty($top_conditioning)){
		$form[$workout[0]->strength_id]['header'] = array(
			'#markup' => '<p>There are no built workouts, please use the Workout Builder to create a workout</p>',
		);	
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

	if(empty($top_ten)){
		$form[$workout[0]->strength_id]['header'] = array(
			'#markup' => '<p>There are no built workouts, please use the Workout Builder to create a workout</p>',
		);	
	  return;
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

	$strength = db_query('SELECT * FROM {workout_builder_strength} where wid=:wid', array(':wid' => $wid));
	$conditioning = db_query('SELECT * FROM {workout_builder_conditioning} where wid=:wid', array(':wid' => $wid));

	if(!empty($strength)){
		$strength_data = array();	
		// put the rows from the db into an array for looping by strength_id
		foreach($strength as $movement){
			// group by the strength id for form building
			$strength_data[$movement->strength_id][] = $movement;
		}
//		drupal_set_message('<pre>' . print_r($strength_data, TRUE) . '</pre>');
	
		// Loop through the different strength_id to populate the form	
		foreach($strength_data as $workout){
			// make the header string
			$movement_str = $workout[0]->sets . 'x' . $workout[0]->reps . ',';
			for($i = 0; isset($workout[$i]); $i++){
				if($i == 0){
					$movement_str = $movement_str . ' ' . $workout[$i]->movement;
				}
				else{
					$movement_str = $movement_str . ' + ' . $workout[$i]->movement;
				}
			}

			if(isset($workout[0]->wgt_style)){
				switch($workout[0]->wgt_style){
					case 'ahap':
						$movement_str = $movement_str . ' AHAP';
						break;
					case 'percentage': 
						$movement_str = $movement_str . ' at ' . $workout[0]->wgt_percentage . '%';
						break;
					case 'weight':
						if(isset($workout[0]->wgt_self_rx)){
							$movement_str = $movement_str . ' at ' . $workout[0]->wgt_self_rx . ' lbs';
							break;
						}
						else{
							$movement_str = $movement_str . ' at ' . $workout[0]->wgt_men . '/' . $workout[0]->wgt_women . ' lbs';
							break;	
						}
				}
			}
			
			if(isset($workout[0]->clock)){
				$movement_str = $movement_str . ' on a ' . $workout[0]->clock . ' clock';
			}

			$form[$workout[0]->strength_id] = array(
				'#type' => 'fieldset',
				'#title' => t('@header', array('@header' => $movement_str)),
				'#collapsible' => TRUE,
				'#collapsed' => TRUE,
			);

			// print the workout line
			$form[$workout[0]->strength_id]['header'] = array(
				'#markup' => '<p>' . $movement_str . '</p>',
			);	
	
			//drupal_set_message('<pre>' . print_r($movement_str, TRUE) . '</pre>');
			// constuct boxes based on sets for weight/count input
			$sets = $workout[0]->sets;
			$reps = $workout[0]->reps; // use reps to populate drop down for failed reps
			$reps_array = array();

			for($j = 1; $j <= $reps; $j++){
				$reps_array[$j] = $j;
			}

			for($k = 1; $k <= $sets; $k++){
				$form[$workout[0]->strength_id][$k]['set'] = array(
					'#type' => 'textfield',
					'#title' => t('Set #@num', array('@num' => $k)),
				);
				
				$form[$workout[0]->strength_id][$k]['fail'] = array(
					'#type' => 'checkbox',
					'#title' => t('Fail on a rep?'),
				);
		
//				$visibility = $workout[0]->strength_id . '[' . $k . '][fail]';
	
				$form[$workout[0]->strength_id][$k]['fail_rep'] = array(
					'#type' => 'select',
					'#title' => t('Failed on rep:'),
					'#options' => $reps_array,
					'#states' => array(
						'visible' => array(
							':input[name="' . $workout[0]->strength_id . '[' . $k . '][fail]"]' => array('checked' => TRUE),
						),
					),
				);				
			}
		}	
	}	
	return $form;
}

function convert_timestamp($timestamp){
	$format = 'D d M Y';
	return date($format, $timestamp);
}

