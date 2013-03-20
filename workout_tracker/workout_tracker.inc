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
		$form['header'] = array(
			'#markup' => '<p>There are no workouts built, please use the Workout Builder to create a workout</p>',
		);	
		return $form;
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
		$form['header'] = array(
			'#markup' => '<p>There are no workouts built, please use the Workout Builder to create a workout</p>',
		);	
	  return $form;
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
		'#title' => t('Choose a workout'),
		'#default_value' => key($top_ten),
	);	

	$form['next'] = array(
		'#type' => 'submit',
		'#value' => t('Track Workout'),
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
	$strength_notes_results = db_query('SELECT * FROM {workout_builder_strength_notes} where wid=:wid', array(':wid' => $wid));
	$conditioning_notes_results = db_query('SELECT * FROM {workout_builder_conditioning_notes} where wid=:wid', array(':wid' => $wid));

	if(!empty($strength)){
		$strength_data = array();	
	
		// put the rows from the db into an array for looping by strength_id
		foreach($strength as $movement){
			// group by the strength id for form building
			$strength_data[$movement->strength_id][] = $movement;
		}

		// group notes for each workout
		foreach($strength_notes_results as $note){
			$strength_notes[$note->strength_id] = $note;
		}

//		drupal_set_message('<pre>' . print_r($strength_notes, TRUE) . '</pre>');
	
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
						if($workout[0]->wgt_units == 0){
							$units = 'lbs';
						}
						else{
							$units = 'kg';
						}

						if(isset($workout[0]->wgt_self_rx)){
							$movement_str = $movement_str . ' at ' . $workout[0]->wgt_self_rx . ' ' . $units;
							break;
						}
						else{
							$movement_str = $movement_str . ' at ' . $workout[0]->wgt_men . '/' . $workout[0]->wgt_women . ' ' . $units;
							break;	
						}
				}
			}
			
			if(isset($workout[0]->clock)){
				$movement_str = $movement_str . ' on a ' . $workout[0]->clock . ' clock';
			}

			$form['strength'][$workout[0]->strength_id] = array(
				'#type' => 'fieldset',
				'#title' => t('@header', array('@header' => $movement_str)),
				'#collapsible' => TRUE,
				'#collapsed' => TRUE,
			);

			// print the workout line
			$form['strength'][$workout[0]->strength_id]['header'] = array(
				'#markup' => '<p>' . $movement_str . '</p>',
			);	

			if(isset($strength_notes[$workout[0]->strength_id]->notes)){
				$form['strength'][$workout[0]->strength_id]['strength_notes'] = array(
					'#markup' => '<p>' . $strength_notes[$workout[0]->strength_id]->notes . '</p>',
				);	
			}
	
			//drupal_set_message('<pre>' . print_r($movement_str, TRUE) . '</pre>');

			// constuct boxes based on sets for weight/count input
			$sets = $workout[0]->sets;
			$reps = $workout[0]->reps; // use reps to populate drop down for failed reps
			$reps_array = array();

			for($j = 1; $j <= $reps; $j++){
				$reps_array[$j] = $j;
			}

			for($k = 1; $k <= $sets; $k++){
				$form['strength'][$workout[0]->strength_id][$k]['set'] = array(
					'#type' => 'textfield',
					'#title' => t('Set #@num Weight/Count', array('@num' => $k)),
				);
				
				$form['strength'][$workout[0]->strength_id][$k]['fail'] = array(
					'#type' => 'checkbox',
					'#title' => t('Fail on a rep?'),
				);
		
//				$visibility = $workout[0]->strength_id . '[' . $k . '][fail]';
	
				$form['strength'][$workout[0]->strength_id][$k]['fail_rep'] = array(
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

	if(!empty($conditioning)){
		$conditioning_data = array();	
	
		// put the rows from the db into an array for looping by conditioning_id
		foreach($conditioning as $movement){
			// group by the conditioning id for form building
			$conditioning_data[$movement->conditioning_id][] = $movement;
		}

		// group notes for each workout
		foreach($conditioning_notes_results as $note){
			$conditioning_notes[$note->conditioning_id] = $note;
		}

		// Loop through the different strength_id to populate the form	
		foreach($conditioning_data as $workout){
			// $workout is an array of movements all with the same conditioning_id

			$workout_str = '';
			$fieldset_title = '';
			if($workout[0]->style == 'amrap'){
				$movement_str = $workout[0]->duration . " minute AMRAP:<br>";
				$fieldset_title = substr($movement_str, 0, strlen($movement_str) - 5);
			}
			elseif($workout[0]->style == 'for_time'){
				$movement_str = $workout[0]->rounds . " Rounds For Time, " . $workout[0]->cutoff . " minute cutoff:<br>";
				$fieldset_title = substr($movement_str, 0, strlen($movement_str) - 5);
			}
			elseif($workout[0]->style == 'intervals' && $workout[0]->rounds > 1){
				$movement_str = $workout[0]->rounds . " Rounds of:<br>";
				$fieldset_title = 'Intervals';
			}

			// build the workout string
			for($i = 0; isset($workout[$i]); $i++){
				if($workout[$i]->style == 'amrap' || $workout[$i]->style == 'for_time'){
					if(!empty($workout[$i]->reps)){
						$movement_str = $movement_str . $workout[$i]->reps . " "  . $workout[$i]->movement;
					}
					else{
						$movement_str = $movement_str . $workout[$i]->movement;
					}
					if($workout[$i]->wgt_choice == 'weight'){
						if($workout[$i]->wgt_style == 'weight'){
							if($workout[$i]->wgt_units == 0){
								$units = 'lbs';
							}
							else{
								$units = 'kg';
							}

							if(!empty($workout[$i]->wgt_self_rx)){
								$movement_str = $movement_str . ", " . $workout[$i]->wgt_self_prescribed . " " . $units . "<br>";
							}
							else{
								$movement_str = $movement_str . ", " . $workout[$i]->wgt_men . '/' . $workout[$i]->wgt_women . " " . $units . "<br>";
							}
						}
						elseif($workout[$i]->wgt_style == 'percentage'){					
							$movement_str = $movement_str . ", " . $workout[$i]->wgt_percentage . "%<br>";
						}
					}	
					elseif($workout[$i]->wgt_choice == 'distance'){
						if(!empty($workout[$i]->distance_self_rx)){
							switch($workout[$i]->distance_units){
								case 0: // meters
									$movement_str = $movement_str . ", " . $workout[$i]->distance_self_rx . " meters<br>";
									break;
								case 1: // inches
									$movement_str = $movement_str . ", " . $workout[$i]->distance_self_rx . " inches<br>";
									break;
								case 2: // cal
									$movement_str = $movement_str . ", " . $workout[$i]->distance_self_rx . " cal<br>";
									break;
							}
						}
						else {	
							switch($workout[$i]->distance_units){
								case 0: // meters
									$movement_str = $movement_str . ", " . $workout[$i]->distance_men . '/' . $workout[$i]->distance_women . " meters<br>";
									break;
								case 1: // inches
									$movement_str = $movement_str . ", " . $workout[$i]->distance_men . '/' . $workout[$i]->distance_women  . " inches<br>";
									break;
								case 2: // cal
									$movement_str = $movement_str . ", " . $workout[$i]->distance_men . '/' . $workout[$i]->distance_women  . " cal<br>";
									break;
							}
						}
					}
					else {
						$movement_str = $movement_str . "<br>";
					}
				}

				// TODO Interval workout building starts here -----
				elseif($workout[$i]->style == 'intervals'){
					$movement_str = $movement_str . $workout[$i]->interval_time . " seconds of " . $workout[$i]->movement;
					if($workout[$i]->wgt_choice == 'weight'){					
						if($workout[$i]->wgt_style == 'weight'){
							if(!empty($workout[$i]->wgt_self_rx)){
								$movement_str = $movement_str . ", " . $workout[$i]->wgt_self_prescribed . " lbs<br>";
							}
							else{
								$movement_str = $movement_str . ", " . $workout[$i]->wgt_men . "/" . $workout[$i]->wgt_women . " lbs<br>";
							}
						}
						elseif($workout[$i]->wgt_style == 'percentage'){					
							$movement_str = $movement_str . ", " . $workout[$i]->wgt_percentage . "%<br>";
						}
					}
					elseif($workout[$i]->wgt_choice == 'distance'){
						if(!empty($workout[$i]->distance_self_rx)){
							switch($workout[$i]->distance_units){
								case 0: // meters
									$movement_str = $movement_str . ", " . $workout[$i]->distance_self_rx . " meters<br>";
									break;
								case 1: // inches
									$movement_str = $movement_str . ", " . $workout[$i]->distance_self_rx . " inches<br>";
									break;
								case 2: // cal
									$movement_str = $movement_str . ", " . $workout[$i]->distance_self_rx . " cal<br>";
									break;
							}
						}
						else {	
							switch($workout[$i]->distance_units){
								case 0: // meters
									$movement_str = $movement_str . ", " . $workout[$i]->distance_men . "/" . $workout[$i]->distance_women . " meters<br>";
									break;
								case 1: // inches
									$movement_str = $movement_str . ", " . $workout[$i]->distance_men . "/" . $workout[$i]->distance_women  . " inches<br>";
									break;
								case 2: // cal
									$movement_str = $movement_str . ", " . $workout[$i]->distance_men . "/" . $workout[$i]->distance_women  . " cal<br>";
									break;
							}
						}
					}
					else {
						$movement_str = $movement_str . "<br>";
					}
				}
			}
			//drupal_set_message('<pre>' . print_r($movement_str, TRUE) . '</pre>');
			// Now that the string is done, build the fieldset to display it.
			$form['conditioning'][$workout[0]->conditioning_id] = array(
					'#type' => 'fieldset',
					'#title' => t('@header', array('@header' => $fieldset_title)),
					'#collapsible' => TRUE,
					'#collapsed' => TRUE,
				);

				// print the workout line
				$form['conditioning'][$workout[0]->conditioning_id]['header'] = array(
					'#markup' => '<p>' . $movement_str . '</p>',
				);	

				if(isset($conditioning_notes[$workout[0]->conditioning_id]->notes)){
					$form['conditioning'][$workout[0]->conditioning_id]['conditioning_notes'] = array(
						'#markup' => '<p>' . $conditioning_notes[$workout[0]->conditioning_id]->notes . '</p>',
					);	
				}
			
				if($workout[0]->style == 'amrap'){	
					$form['conditioning'][$workout[0]->conditioning_id]['amrap_rounds'] = array(
						'#type' => 'textfield',
						'#title' => t('Rounds Completed'),
						'#size' => 3,
						'#maxlength' => 3,
					);

					$form['conditioning'][$workout[0]->conditioning_id]['amrap_reps'] = array(
						'#type' => 'textfield',
						'#title' => t('Extra Reps'),
						'#size' => 3,
						'#maxlength' => 3,
					);
				}
			
				if($workout[0]->style == 'for_time'){
					$form['conditioning'][$workout[0]->conditioning_id]['for_time'] = array(
						'#type' => 'textfield',
						'#title' => t('Time'),
						'#size' => 5,
						'#maxsize' => 5,
					); 
				}

			// TODO add stuff for intervals

		}
	}
	
	return $form;
}

function convert_timestamp($timestamp){
	$format = 'D d M Y';
	return date($format, $timestamp);
}

