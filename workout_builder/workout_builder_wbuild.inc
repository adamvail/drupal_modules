<?php

function workout_builder_wbuilder($form, &$form_state){

  global $user;
	
  $form['#tree'] = TRUE;

  $form['description'] = array(
    '#type' => 'item',
    '#title' => t('Strength Portion'),
  );

  if(!empty($form_state['page_num']) && $form_state['page_num'] == 2){
    return workout_builder_wbuild_page_two($form, $form_state);
  }

  if(!empty($form_state['page_values'][1])){
    $form_state['values'] = $form_state['page_values'][1];
  }


  $form_state['page_num'] = 1;

	// initialize to a single strength workout
  if(empty($form_state['num_strength_wo'])){
		$form_state['num_strength_wo'] = 1;
	}

	// needed for initialization, when an addition strength workout
	// is added then there needs to be one movement to start out with.
  // addition and deletion of all other movements in this additional
	// workout are handled in the button handlers.
	// reset this value when deleting a workout, look at handler for more details
/*  $num_workouts = $form_state['num_strength_wo'];
  if(empty($form_state['num_strength'][$num_workouts])){
    $form_state['num_strength'][$num_workouts] = 1;
  }  
*/
	
	$format = 'n/j/Y';
	$default_date = date($format, time());

	$form['date'] = array(
		'#type' => 'fieldset',
		'#title' => t('Workout Date'),
		'#collapsible' => TRUE,
		'#collapsed' => FALSE,
	);

	$form['date']['workout_date'] = array(
		'#type' => 'date_select',
		'#title' => t('Select a date for this workout'),
		'#default_value' => $default_date,
		'#date_format' => $format,
		'#date_label_position' => 'within',	
	);

	for($k = 1; $k <= $form_state['num_strength_wo']; $k++){

		if(empty($form_state[$k]['num_movements'])){
			$form_state[$k]['num_movements'] = 1;
		}

		$form['strength_wo'][$k] = array(
			'#type' => 'fieldset',
			'#title' => t('Workout #@num', array('@num' => $k)),
			'#collapsible' => TRUE,
			'#collapsed' => TRUE,
		);

		// don't use the #name field in any of these
		// IT BREAKS F****** EVERYTHING!!!!! aka an hour of my life I can't have back
		$form['strength_wo'][$k]['clock'] = array(
			'#type' => 'textfield',
			'#title' => t('Clock'),
			'#description' => t('Time per set in minutes<br>e.g. 1 or 1:30 or :30'),
			'#size' => 5,
			'#maxlength' => 10,
		);

		$form['strength_wo'][$k]['rep_scheme'] = array(
			'#type' => 'textfield',
			'#title' => t('Rep Scheme'),
			'#description' => t('e.g. 5x3 or 5-3-1-1'),
			'#size' => 20,
			'#maxlength' => 20,
		);

/*
		$form['strength_wo'][$k]['sets'] = array(
			'#type' => 'textfield',
			'#title' => t('Number of Sets'),
			'#description' => t('Enter number of sets'),
			'#size' => 2,
			'#maxlength' => 2,
		);

		$form['strength_wo'][$k]['reps'] = array(
			'#type' => 'textfield',
			'#title' => t('Number of Reps'),
			'#description' => t('Enter number of reps per set. (or 0 for max reps)'),
			'#size' => 3,
			'#maxlength' => 3,
		);
*/
		$form['strength_wo'][$k]['weight'] = array(
			'#type' => 'fieldset',
			'#title' => t('Weight'),
			'#collapsible' => TRUE,
			'#collapsed' => TRUE,
		);

		$form['strength_wo'][$k]['weight']['style'] = array(
			'#type' => 'radios',
//      '#options' => drupal_map_assoc(array(t('Percentage'), t('Weight'))),
			'#options' => array(
				'percentage' => t('Percentage'), 			// 0
				'weight' => t('Weight'),							// 1
				'ahap' => t('As Heavy As Possible'),	// 2
			),
			'#title' => t('Percentage of max or a specific weight?'),
		);

		$form['strength_wo'][$k]['weight']['percentage'] = array(
			'#type' => 'textfield',
			'#description' => t('Enter prescribed percentage of max.<br>If you want to specify a different percentage per set, seperate by \'-\'.'),
			'#size' => 10,
			'#maxlength' => 50,
			'#states' => array(
				'visible' => array(
					':input[name="strength_wo[' . $k . '][weight][style]"]' => array('value' => 'percentage'),
				),
			),
		);

		$form['strength_wo'][$k]['weight']['units'] = array(
			'#type' => 'select',
			'#description' => t('Lbs or KG'),
			'#options' => array(t('lbs'), t('kg')),
			'#states' => array(
				'visible' => array(
					':input[name="strength_wo[' . $k . '][weight][style]"]' => array('value' => 'weight'),
				),
			),
		);

		$form['strength_wo'][$k]['weight']['men'] = array(
			'#type' => 'textfield',
			'#title' => t('Men'),
			'#description' => t('Enter prescribed weight for men'),
			'#size' => 3,
			'#maxlength' => 5,
			'#access' => in_array('coach', array_values($user->roles)),
			'#states' => array(
				'visible' => array(
					':input[name="strength_wo[' . $k . '][weight][style]"]' => array('value' => 'weight'),
				),
			),
		);

		$form['strength_wo'][$k]['weight']['women'] = array(
			'#type' => 'textfield',
			'#title' => t('Women'),
			'#description' => t('Enter prescribed weight for women'),
			'#size' => 3,
			'#maxlength' => 5,
			'#access' => in_array('coach', array_values($user->roles)),
			'#states' => array(
				'visible' => array(
					':input[name="strength_wo[' . $k . '][weight][style]"]' => array('value' => 'weight'),
				),
			),
		);

		$form['strength_wo'][$k]['weight']['self_prescribed'] = array(
			'#type' => 'textfield',
			'#description' => t('Enter prescribed weight.'),
			'#size' => 3,
			'#maxlength' => 5,
			'#access' => in_array('athlete', array_values($user->roles)),
			'#states' => array(
				'visible' => array(
					':input[name="strength_wo[' . $k . '][weight][style]"]' => array('value' => 'weight'),
				),
			),
		);

		for($i = 1; $i <= $form_state[$k]['num_movements']; $i++){
			$form['strength_wo'][$k]['strength_exercises'][$i] = array(
				'#type' => 'fieldset',
				'#title' => t('Movement'),
				'#collapsible' => TRUE,
				'#collapsed' => TRUE,
			);

/*			$form['strength_wo'][$k]['strength_exercises'][$i]['reps'] = array(
				'#type' => 'textfield',
				'#title' => t('Reps'),
				'#description' => t('Enter number of reps for this movement.'),
				'#size' => 3,
				'#maxlength' => 3,
				'#default_value' => '1',
			);
*/
			$form['strength_wo'][$k]['strength_exercises'][$i]['lift'] = array(
				'#type' => 'textfield',
				'#title' => t('Movement'),
				'#description' => t('Enter the movement<br>EX1) Clean <br>EX2) Clean + Jerk <br>EX3) 2[deadlift + hang clean] + Jerk<br>To specify complex movements, read the help.'),
				'#size' => 20,
				'#maxlength' => 100,
			);
		}

		$form['strength_wo'][$k]['notes'] = array(
			'#type' => 'textarea',
			'#title' => t('Notes'),
		);
/*
		$form['strength_wo'][$k]['add_strength'] = array(
			'#type' => 'submit',
			'#name' => $k,
			'#value' => t('Add another movement'),
			'#submit' => array('workout_builder_wbuild_add_strength'),
		);
*/
		if($form_state[$k]['num_movements'] > 1){
			$form['strength_wo'][$k]['remove_strength'] = array(
				'#type' => 'submit',
				'#name' => $k,
				'#value' => t('Remove last strength movement'),
				'#submit' => array('workout_builder_wbuild_remove_strength'),
				'#limit_validate_errors' => array(),
			);
		}
	}

	$form['add_strength_wo'] = array(
		'#type' => 'submit',
		'#value' => t('Add another strength workout'),
		'#submit' => array('workout_builder_wbuild_add_strength_wo'),
	);

	if($form_state['num_strength_wo'] > 1){
		$form['remove_strength_wo'] = array(
			'#type' => 'submit',
			'#value' => t('Remove last strength workout'),
			'#submit' => array('workout_builder_wbuild_remove_strength_wo'),
			'#limit_validate_errors' => array(),
		);
	}

  $form['next'] = array(
    '#type' => 'submit',
    '#value' => 'Add Conditioning',
    '#submit' => array('workout_builder_wbuild_next_submit'),
    '#validate' => array('workout_builder_wbuild_next_validate'),
  );

  return $form;
}

function workout_builder_wbuild_add_strength($form, &$form_state){
	$index = $form_state['triggering_element']['#name'];
  $form_state[$index]['num_movements']++;
  $form_state['rebuild'] = TRUE;
}

function workout_builder_wbuild_remove_strength($form, &$form_state){
	$index = $form_state['triggering_element']['#name'];
  if($form_state[$index]['num_movements'] > 1){
    $form_state[$index]['num_movements']--;
  }
  $form_state['rebuild'] = TRUE;
}

function workout_builder_wbuild_add_strength_wo($form, &$form_state){
  $form_state['num_strength_wo']++;
  $form_state['rebuild'] = TRUE;
}

function workout_builder_wbuild_remove_strength_wo($form, &$form_state){
  if($form_state['num_strength_wo'] > 1){
		$index = $form_state['num_strength_wo'];
    $form_state['num_strength_wo']--;
		// reset start value so when user adds a new workout after
		// just deleting one, it only has a single movement
		$form_state[$index]['num_movements'] = 1;
  }
  $form_state['rebuild'] = TRUE;
}


function workout_builder_wbuild_next_submit($form, &$form_state){
  $form_state['page_values'][1] = $form_state['values'];

  if(!empty($form_state['page_values'][2])){
    $form_state['values'] = $form_state['page_values'][2];
  }

  $form_state['page_num'] = 2;
  $form_state['rebuild'] = TRUE;
}

function workout_builder_wbuild_next_validate($form, &$form_state){
	// cycle through all the specified strength workouts
	// check to make sure everything is numeric
	// don't check the movement since it can be a mix of numerics and strings
	// e.g. Hang Clean or T2B/K2E, both a valid, eventually move to taxonomy
	// populated element
	for($i = 1; isset($form_state['values']['strength_wo'][$i]); $i++){
		$workout = $form_state['values']['strength_wo'][$i];

		if(!_validate_numeric($workout['clock'])){
		//	form_set_error($workout['clock'], t('Workout #'. $i .': Clock field must be a number'));	
		}

		if(isset($workout['rep_scheme'])){
			$combined_scheme = FALSE;
			$singular_scheme = FALSE;

			if(strpos($workout['rep_scheme'], ",") !== FALSE){
				form_set_error($workout['rep_scheme'], t('Can only specify one style of rep scheme. e.g. 5x3 OR 5-3-2-1'));
			}
			if(strpos($workout['rep_scheme'], "x") !== FALSE){
				$combined_scheme = TRUE;
			}
			if(strpos($workout['rep_scheme'], "-") !== FALSE){
				$singular_scheme = TRUE;
			}
			
			if($combined_scheme == TRUE && $singular_scheme == TRUE){
				form_set_error($workout['rep_scheme'], t('Can only specify one style of rep scheme. e.g. 5x3 OR 5-3-2-1'));
			}
		}

		$movement = $workout['strength_exercises'][1]['lift'];
		if(substr_count($movement, "[") > 1 || substr_count($movement, "]") > 1){
			form_set_error($movement, t('Can only have one complex movement (e.g. movement enclosed in []) at this time'));
		}

		$weight = $workout['weight'];

		if(!empty($weight['percentage'])){
			// make sure the number of sets and the number of percentages specified are the same.
			if(strpos($weight['percentage'], ",") !== FALSE){
				form_set_error($weight['percentage'], t('Percentages must be separated by \'-\''));
			}
			if(strpos($workout['rep_scheme'], "x") !== FALSE){
				$num_sets = explode("x", $workout['rep_scheme']);
				$num_sets = trim($num_sets[0]); 
			}
			elseif(strpos($workout['rep_scheme'], "-") !== FALSE){
				$num_sets = explode("-", $workout['rep_scheme']);
				$num_sets = sizeof($num_sets);
			}
			else {
				form_set_error($workout['rep_scheme'], t('Unknown rep scheme'));
			}
			$percentage_scheme = explode("-", $weight['percentage']);
			if($num_sets != sizeof($percentage_scheme)){
				form_set_error($weight['percentage'], t('Must specify the same number of sets and percentages'));
			}
		}

		if(!empty($weight['men'])){
			if(!_validate_numeric($weight['men'])){
				form_set_error($weight['men'], t('Workout #'. $i .': Men\'s weight field must be a number'));	
			}

			if(!_validate_range($weight['men'], 0, NULL)){
				form_set_error($weight['men'], t('Workout #'. $i .': Men\'s weight field must be greater than 0'));	
			}
		}

		if(!empty($weight['women'])){
			if(!_validate_numeric($weight['women'])){
				form_set_error($weight['women'], t('Workout #'. $i .': Women\'s weight field must be a number'));	
			}

			if(!_validate_range($weight['women'], 0, NULL)){
				form_set_error($weight['women'], t('Workout #'. $i .': Women\'s weight field must be greater than 0'));	
			}
		}

		if(!empty($weight['self_prescribed'])){
			if(!_validate_numeric($weight['self_prescribed'])){
				form_set_error($weight['self_prescribed'], t('Workout #'. $i .': Weight field must be a number'));	
			}		

			if(!_validate_range($weight['self_prescribed'], 0, NULL)){
				form_set_error($weight['self_prescribed'], t('Workout #'. $i .': Weight field must be greater than 0'));	
			}
		}


		
//		drupal_set_message('<pre>' . print_r($form_state['values']['strength_wo'][$i]['clock'], TRUE) . '</pre>');	
	}
}

function _validate_numeric($value){
	if(!empty($value) && !is_numeric($value)){
			return FALSE;
	}
	else{
		return TRUE;
	}
}

/**
* Return if value from form in in between the range inclusive
*/

function _validate_range($value, $start, $finish){
	$number = (int)$value;

	if($finish == NULL && $number >= $start){
		return TRUE;
	}
	elseif($number >= $start && $number <= $finish){
		return TRUE;
	}
	else{
		return FALSE;
	}
}

function workout_builder_wbuild_page_two($form, &$form_state){
  global $user;

  $form['description'] = array(
    '#type' => 'item',
    '#title' => t('Conditioning Portion'),
  );

  if(empty($form_state['num_cond_wo'])){
    $form_state['num_cond_wo'] = 1;
  }
 
  $num_workouts = $form_state['num_cond_wo'];
  if(empty($form_state['num_condition'][$num_workouts])){
    $form_state['num_condition'][$num_workouts] = 1;
  }  

  for($k = 1; $k <= $form_state['num_cond_wo']; $k++){
    $form['cond_workout'][$k] = array(
      '#type' => 'fieldset',
      '#title' => t('Workout #@num', array('@num' => $k)),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );

    $form['cond_workout'][$k]['condition_exercises']['style'] = array(
      '#type' => 'radios',
      '#options' => array(
				'amrap' => t('AMRAP'),
				'for_time' => t('For Time'),
				'intervals' => t('Intervals'),
				'rest' => t('Rest'),
      ),
      '#title' => t('Choose the style of conditioning workout'),
    );
	
		$form['cond_workout'][$k]['condition_exercises']['rest'] = array(
			'#type' => 'textfield',
      '#title' => t('Duration'),
      '#description' => t('Enter rest time (2:30 or :30 for seconds)'),
      '#size' => 5,
      '#maxlength' => 10,
      '#states' => array(
				'visible' => array(
	    		':input[name="cond_workout[' . $k . '][condition_exercises][style]"]' => array('value' => 'rest'),
				),
      ),
		);

    $form['cond_workout'][$k]['condition_exercises']['duration'] = array(
      '#type' => 'textfield',
      '#title' => t('Duration'),
      '#description' => t('Enter timing in minutes'),
      '#size' => 5,
      '#maxlength' => 10,
      '#states' => array(
				'visible' => array(
	    		':input[name="cond_workout[' . $k . '][condition_exercises][style]"]' => array('value' => 'amrap'),
				),
      ),
    );

    $form['cond_workout'][$k]['condition_exercises']['rounds'] = array(
      '#type' => 'textfield',
      '#title' => t('Rounds'),
      '#description' => t('Number of rounds<br>e.g. 5 or 21-15-9'),
      '#size' => 10,
      '#maxlength' => 30,
      '#states' => array(
				'visible' => array(
	    		':input[name="cond_workout[' . $k . '][condition_exercises][style]"]' => array(
	    			array('value' => 'for_time'),
	    			array('value' => 'intervals'),
	  			),
				),
      ),
    );

		$form['cond_workout'][$k]['condition_exercises']['interval_on_clock'] = array(
			'#type' => 'checkbox',
			'#title' => t('Start rounds on a clock?'),
      '#states' => array(
				'visible' => array(
	    		':input[name="cond_workout[' . $k . '][condition_exercises][style]"]' => array('value' => 'intervals'),
				),
      ),
		);

		$form['cond_workout'][$k]['condition_exercises']['interval_style'] = array(
			'#type' => 'select',
			'#options' => array(t('For Time'), t('For Work')),
			'#description' => t('Is the interval meant to be completed for amount of work done or <br>for how quickly the work is done?'),
      '#states' => array(
				'visible' => array(
	    		':input[name="cond_workout[' . $k . '][condition_exercises][interval_on_clock]"]' => array('checked' => TRUE),
				),
      ),
		);

		$form['cond_workout'][$k]['condition_exercises']['interval_clock'] = array(
      '#type' => 'textfield',
      '#title' => t('Clock'),
      '#description' => t('Enter timing in minutes'),
      '#size' => 5,
      '#maxlength' => 10,
      '#states' => array(
				'visible' => array(
	    		':input[name="cond_workout[' . $k . '][condition_exercises][interval_on_clock]"]' => array('checked' => TRUE),
				),
      ),
		);

    $form['cond_workout'][$k]['condition_exercises']['cutoff'] = array(
      '#type' => 'textfield',
      '#title' => t('Time Cutoff'),
      '#description' => t('Enter time cutoff in minutes'),
      '#size' => 5,
      '#maxlength' => 10,
      '#states' => array(
				'visible' => array(
	  			':input[name="cond_workout[' . $k . '][condition_exercises][style]"]' => array('value' => 'for_time'),
				),
      ),
    );

		$form['cond_workout'][$k]['condition_exercises']['workout'] = array(
			'#type' => 'textarea',
			'#title' => t('Workout'),
			'#description' => t('One movement per line'),
			'#states' => array(
				'visible' => array(
	    		':input[name="cond_workout[' . $k . '][condition_exercises][style]"]' => array(
						array('value' => 'amrap'),
	    			array('value' => 'for_time'),
	    			array('value' => 'intervals'),
	  			),
				),
      ),
		);

		$form['cond_workout'][$k]['condition_exercises']['notes'] = array(
			'#type' => 'textarea',
			'#title' => t('Notes'),
			'#states' => array(
				'visible' => array(
	    		':input[name="cond_workout[' . $k . '][condition_exercises][style]"]' => array(
						array('value' => 'amrap'),
	    			array('value' => 'for_time'),
	    			array('value' => 'intervals'),
	  			),
				),
      ),
		);
/*
    $form['cond_workout'][$k]['add_cond_movement'] = array(
      '#type' => 'submit',
      '#name' => $k,
      '#value' => t('Add another movement'),
      '#submit' => array('workout_builder_wbuild_add_condition'),
			'#states' => array(
	  			'visible' => array(
	    			':input[name="cond_workout[' . $k . '][condition_exercises][style]"]'=> array(
	      			array('value' => 'amrap'), 
	      			array('value' => 'for_time'), 
	      			array('value' => 'intervals'), 
	    			),
	  			),
				),
    );

    if($form_state['num_condition'][$k] > 1){
      $form['cond_workout'][$k]['remove_cond_movement'] = array(
				'#type' => 'submit',
				'#name' => $k,
				'#value' => t('Remove last movement'),
				'#submit' => array('workout_builder_wbuild_remove_condition'),
				'#limit_validate_errors' => array(),
      );
    }
*/
  }

  $form['back'] = array(
    '#type' => 'submit',
    '#value' => t('Back to Strength'),
    '#submit' => array('workout_builder_wbuild_page_two_back'),
    '#limit_validation_errors' => array(),
  );

  $form['add_cond_wo'] = array(
    '#type' => 'submit',
    '#value' => t('Add another conditioning workout'),
    '#submit' => array('workout_builder_wbuild_add_cond_wo'),
  );

  if($form_state['num_cond_wo'] > 1){
    $form['remove_cond_wo'] = array(
      '#type' => 'submit',
      '#value' => t('Remove last conditioning workout'),
      '#submit' => array('workout_builder_wbuild_remove_cond_wo'),
      '#limit_validate_errors' => array(),
    );
  }

  $form['build'] = array(
    '#type' => 'submit',
    '#value' => t('Build Workout'),
		'#validate' => array('workout_builder_wbuild_page_two_build_validate'),
    '#submit' => array('workout_builder_wbuild_page_two_build_submit'),
  );

  return $form;
}

function workout_builder_wbuild_add_condition($form, &$form_state){
  $index = $form_state['triggering_element']['#name'];
  $form_state['num_condition'][$index]++;
  $form_state['rebuild'] = TRUE;
}

function workout_builder_wbuild_remove_condition($form, &$form_state){
  $index = $form_state['triggering_element']['#name'];
  if($form_state['num_condition'][$index] > 1){
    $form_state['num_condition'][$index]--;
  }

  $form_state['page_values'][2] = $form_state['values'];
  

  $form_state['page_num'] = 2;
  $form_state['rebuild'] = TRUE;
}

function workout_builder_wbuild_add_cond_wo($form, &$form_state){
  $form_state['num_cond_wo']++;
  $form_state['rebuild'] = TRUE;
}

function workout_builder_wbuild_remove_cond_wo($form, &$form_state){
  if($form_state['num_cond_wo'] > 1){
		$index = $form_state['num_cond_wo'];
    $form_state['num_cond_wo']--;
		// reset start value so when user adds a new workout after
		// just deleting one, it only has a single movement
		$form_state['num_condition'][$index] = 1;
  }

  $form_state['page_values'][2] = $form_state['values'];
  

  $form_state['page_num'] = 2;
  $form_state['rebuild'] = TRUE;
}

function workout_builder_wbuild_page_two_back($form, &$form_state){
  $form_state['values'] = $form_state['page_values'][1];
  $form_state['page_num'] = 1;
  $form_state['rebuild'] = TRUE;
}

function workout_builder_wbuild_page_two_build_validate($form, &$form_state){
	// validate errthang
//	drupal_set_message('<pre>' . print_r($form_state['values']['cond_workout'], TRUE) . '</pre>');	
//		drupal_set_message('<pre>' . print_r($workout, TRUE) . '</pre>');	

	// need to rethink how to do all of this
	
	return;


	for($i = 1; isset($form_state['values']['cond_workout'][$i]); $i++){
		$workout = $form_state['values']['cond_workout'][$i]['condition_exercises'];

		if(!empty($workout['duration'])){
	/*		if(!_validate_numeric($workout['duration'])){
				form_set_error($workout['duration'], t('Workout #'. $i .': Duration time field must be a number'));	
			}

			if(!_validate_range($workout['duration'], 0, NULL)){
				form_set_error($workout['duration'], t('Workout #'. $i .': Duration time field must be greater than 0'));	
			}
*/
		}

		if(!empty($workout['rounds'])){
			if(!_validate_numeric($workout['rounds'])){
				form_set_error($workout['rounds'], t('Workout #'. $i .': Number of rounds must be a number'));	
			}

			if(!_validate_range($workout['rounds'], 1, NULL)){
				form_set_error($workout['rounds'], t('Workout #'. $i .': Number of rounds must be at least 1'));	
			}
		}

		if(!empty($workout['cutoff'])){
			if(!_validate_numeric($workout['cutoff'])){
				form_set_error($workout['cutoff'], t('Workout #'. $i .': Cutoff time must be a number'));	
			}

			if(!_validate_range($workout['cutoff'], 1, NULL)){
				form_set_error($workout['cutoff'], t('Workout #'. $i .': Cutoff time must be at least 1 minute'));	
			}
		}

		for($k = 1; isset($workout[$k]); $k++){
			$movement = $workout[$k];

			if(!empty($movement['reps'])){
				if(!_validate_numeric($movement['reps'])){
					form_set_error($movement['reps'], t('Workout #'. $i .': Movement #' . $k . ': Reps must be a number'));	
				}

				if(!_validate_range($movement['reps'], 1, NULL)){
					form_set_error($movement['reps'], t('Workout #'. $i .': Movement #' . $k . ': Reps must be greater than 1'));	
				}
			}

			if(!empty($movement['interval_time'])){
		/*		if(!_validate_numeric($movement['interval_time'])){
					form_set_error($movement['interval_time'], t('Workout #'. $i .': Movement #' . $k . ': Interval must be a number'));	
				}

				if(!_validate_range($movement['interval_time'], 1, NULL)){
					form_set_error($movement['interval_time'], t('Workout #'. $i .': Movement #' . $k . ': Interval must be greater than 1 second'));	
				}
*/
			}

			$weight = $movement['weight'];
			if(!empty($weight['percentage'])){
				if(!_validate_numeric($weight['percentage'])){
					form_set_error($weight['percentage'], t('Workout #'. $i .': Movement #' . $k . ': Percentage must be a number'));	
				}

				if(!_validate_range($weight['percentage'], 1, 100)){
					form_set_error($weight['percentage'], t('Workout #'. $i .': Movement #' . $k . ': Percentage must be between 1 - 100'));	
				}
			}

			if(!empty($weight['men'])){
				if(!_validate_numeric($weight['men'])){
					form_set_error($weight['men'], t('Workout #'. $i .': Movement #' . $k . ': Men\'s weight must be a number'));	
				}

				if(!_validate_range($weight['men'], 1, NULL)){
					form_set_error($weight['men'], t('Workout #'. $i .': Movement #' . $k . ': Men\'s weight must be greater than 0'));	
				}
			}

			if(!empty($weight['women'])){
				if(!_validate_numeric($weight['women'])){
					form_set_error($weight['women'], t('Workout #'. $i .': Movement #' . $k . ': Women\'s weight must be a number'));	
				}

				if(!_validate_range($weight['women'], 1, NULL)){
					form_set_error($weight['women'], t('Workout #'. $i .': Movement #' . $k . ': Women\'s weight must be greater than 0'));	
				}
			}

			if(!empty($weight['self_prescribed'])){
				if(!_validate_numeric($weight['self_prescribed'])){
					form_set_error($weight['self_prescribed'], t('Workout #'. $i .': Movement #' . $k . ': Weight must be a number'));	
				}

				if(!_validate_range($weight['self_prescribed'], 1, NULL)){
					form_set_error($weight['self_prescribed'], t('Workout #'. $i .': Movement #' . $k . ': Weight must be greater than 0'));	
				}
			}

			$distance = $movement['distance'];
			if(!empty($distance['men'])){
				if(!_validate_numeric($distance['men'])){
					form_set_error($distance['men'], t('Workout #'. $i .': Movement #' . $k . ': Men\'s distance must be a number'));	
				}

				if(!_validate_range($distance['men'], 1, NULL)){
					form_set_error($distance['men'], t('Workout #'. $i .': Movement #' . $k . ': Men\'s distance must be greater than 0'));	
				}
			}

			if(!empty($distance['women'])){
				if(!_validate_numeric($distance['women'])){
					form_set_error($distance['women'], t('Workout #'. $i .': Movement #' . $k . ': Women\'s distance must be a number'));	
				}

				if(!_validate_range($distance['women'], 1, NULL)){
					form_set_error($distance['women'], t('Workout #'. $i .': Movement #' . $k . ': Women\'s distance must be greater than 0'));	
				}
			}

			if(!empty($distance['self_prescribed'])){
				if(!_validate_numeric($distance['self_prescribed'])){
					form_set_error($distance['self_prescribed'], t('Workout #'. $i .': Movement #' . $k . ': Distance must be a number'));	
				}

				if(!_validate_range($distance['self_prescribed'], 1, NULL)){
					form_set_error($distance['self_prescribed'], t('Workout #'. $i .': Movement #' . $k . ': Distance must be greater than 0'));	
				}
			}
		}
	}
}

function workout_builder_wbuild_page_two_build_submit($form, &$form_state){
	// Put data into the database
  
	// get the highest wid in the table and use one more than it.
	$wid_strength = db_query('SELECT MAX(wid) FROM {workout_builder_strength}')->fetchField();
	$wid_cond = db_query('SELECT MAX(wid) FROM {workout_builder_conditioning}')->fetchField();

	if(isset($wid_strength) || isset($wid_cond)){
		if((int)$wid_strength >= (int)$wid_cond){
			$wid = (int)$wid_strength + 1;
		}
		else{
			$wid = (int)$wid_cond + 1;
		}
	}
	else{
		$wid = 1;
	}
	
	_populate_strength_table($form_state, $wid);
	_populate_conditioning_table($form_state, $wid);

}

function _populate_strength_table(&$form_state, $wid){
	global $user;
	$table = 'workout_builder_strength';
	$table_notes = 'workout_builder_strength_notes';

	$gym = db_query('SELECT gym from {workout_gym_affiliation} where uid=:uid', array(':uid' => $user->uid))->fetchField();

	for($i = 1; isset($form_state['page_values'][1]['strength_wo'][$i]); $i++){
		$workout = $form_state['page_values'][1]['strength_wo'][$i];
		$record = new stdClass();
		$record->wid = $wid;
		$record->date = strtotime($form_state['page_values'][1]['date']['workout_date']);
		//drupal_set_message('<pre>' . print_r(date('n/j/Y', $record->date), TRUE) . '</pre>');
		$record->strength_id = $i;
		$record->creator_id = $user->uid;

		if(isset($gym) && !empty($gym)){
			$record->gym = $gym;
		}

		$workout_exists = TRUE;

		$note = new stdClass();
		$note->wid = $wid;
		$note->strength_id = $i;

		// need an entry for every movement, going to take up a lot of space though...
		for($k = 1; isset($workout['strength_exercises'][$k]); $k++){	
			$movement = $workout['strength_exercises'][$k];

			$record->clock = $workout['clock'];
			$record->rep_scheme = $workout['rep_scheme'];
//			$record->sets = $workout['sets'];
//			$record->reps = $workout['reps'];

			$note->notes = $workout['notes'];

			if($workout['weight']['style'] == 'percentage'){
				$record->wgt_style = 'percentage';
				$percentage = str_replace("%", "", $workout['weight']['percentage']);
				$percentage = str_replace(",", "-", $percentage);
				$record->wgt_percentage = $percentage;
			}
			elseif($workout['weight']['style'] == 'weight'){
				$record->wgt_style = 'weight';				
				$record->wgt_units = $workout['weight']['units'];
				if(in_array('coach', array_values($user->roles))){
					$record->wgt_men = $workout['weight']['men'];
					$record->wgt_women = $workout['weight']['women'];
				}
				else{
					$record->wgt_self_rx = $workout['weight']['self_prescribed'];
				}
			}
			elseif($workout['weight']['style'] == 'ahap'){
				$record->wgt_style = 'ahap';
			}
		
			if(empty($movement['lift'])){
				// This workout isn't being used.
				if(!isset($workout['strength_exercises'][$k-1]['lift'])){
//					drupal_set_message('<pre>' . print_r("NOT adding notes to this workout, strength", TRUE) . '</pre>');
					$workout_exists = FALSE;
				}

				break;
			}	
			else{
				$lift = $movement['lift'];
				if(strtoupper($lift) != $lift){
					$lift = strtolower($lift);
				}
				$record->movement = $lift; 
			}
//			drupal_set_message('<pre>' . print_r($record, TRUE) . '</pre>');

			// insert the record in the table
			if($workout_exists == TRUE){
				drupal_write_record($table, $record);
			}
		}

		// If there was a movement specified then add the note to the database
		if(!empty($note->notes)){
//			drupal_set_message('<pre>' . print_r($note, TRUE) . '</pre>');
			drupal_write_record($table_notes, $note);
		}
	}
}

function _populate_conditioning_table(&$form_state, $wid){
	global $user;
	$table = 'workout_builder_conditioning';
	$table_notes = 'workout_builder_conditioning_notes';
	$gym = db_query('SELECT gym from {workout_gym_affiliation} where uid=:uid', array(':uid' => $user->uid))->fetchField();

//	drupal_set_message('<pre>' . print_r($form_state['values'], TRUE) . '</pre>');
	for($i = 1; isset($form_state['values']['cond_workout'][$i]); $i++){
		$workout = $form_state['values']['cond_workout'][$i]['condition_exercises'];

		$workout_exists = TRUE;
	
		$note = new stdClass();
		$note->wid = $wid;
		$note->conditioning_id = $i;

//		for($k = 1; isset($workout[$k]); $k++){	
			//$movement = $workout[$k];
			$record = new stdClass();
			$record->wid = $wid;
			$record->date = strtotime($form_state['page_values'][1]['date']['workout_date']);
			$record->conditioning_id = $i;
			$record->creator_id = $user->uid;

			if(isset($gym) && !empty($gym)){
				$record->gym = $gym;
			}

			$note->notes = $workout['notes'];

			if($workout['style'] == 'amrap'){
				$record->style = 'amrap';
				$record->duration = $workout['duration'];
				// replace newlines with pipes to make it easier to read in the DB
				$record->workout = str_replace("\n", "<br>", $workout['workout']);
				$exists = TRUE;
			}	
			elseif($workout['style'] == 'for_time'){
				$record->style = 'for_time';
				$record->rounds = $workout['rounds'];
				$record->cutoff = $workout['cutoff'];
				$record->workout = str_replace("\n", "<br>", $workout['workout']);
			}
			elseif($workout['style'] == 'intervals'){
				$record->style = 'intervals';
				$record->rounds = $workout['rounds'];
			//	$record->interval_time = $movement['interval_time'];
				$record->workout = strtolower(str_replace("\n", "<br>", $workout['workout']));
				if(isset($workout['interval_clock']) && !empty($workout['interval_clock'])){
					$record->interval_time = $workout['interval_clock'];
					$record->interval_style = $workout['interval_style'];
				}
			}
			elseif($workout['style'] == 'rest'){
				$record->style = 'rest';
				$record->duration = $workout['rest'];
			}
			else{
				// None of the radios were selected so there must not
				// be a conditioning portion to this workout
				$workout_exists = FALSE;
				break;
			}

			//drupal_set_message('<pre>' . print_r($record, TRUE) . '</pre>');
			drupal_write_record($table, $record);
		}
		if(!empty($note->notes)){
			//drupal_set_message('<pre>' . print_r($note, TRUE) . '</pre>');
			drupal_write_record($table_notes, $note);
		}
//	}
}





