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
			'#description' => t('Time per set in minutes'),
			'#size' => 5,
			'#maxlength' => 10,
		);

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
			'#description' => t('Enter number of reps per set. (or MAX for max reps)'),
			'#size' => 3,
			'#maxlength' => 3,
		);

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
				'percentage' => t('Percentage'),
				'weight' => t('Weight'),
				'ahap' => t('As Heavy As Possible'),
			),
			'#title' => t('Prescribe a percentage of max or a specific weight?'),
		);

		$form['strength_wo'][$k]['weight']['percentage'] = array(
			'#type' => 'textfield',
			'#description' => t('Enter prescribed percentage of max'),
			'#size' => 3,
			'#maxlength' => 5,
			'#states' => array(
				'visible' => array(
					':input[name="strength_wo[' . $k . '][weight][style]"]' => array('value' => 'percentage'),
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
				'#title' => t('Movement #@num', array('@num' => $i)),
				'#collapsible' => TRUE,
				'#collapsed' => TRUE,
			);

			$form['strength_wo'][$k]['strength_exercises'][$i]['lift'] = array(
				'#type' => 'textfield',
				'#title' => t('Movement'),
				'#description' => t('Enter the movement'),
				'#size' => 20,
				'#maxlength' => 40,
			);
		}

		$form['strength_wo'][$k]['add_strength'] = array(
			'#type' => 'submit',
			'#name' => $k,
			'#value' => t('Add another movement'),
			'#submit' => array('workout_builder_wbuild_add_strength'),
		);

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
			form_set_error($workout['clock'], t('Workout #'. $i .': Clock field must be a number'));	
		}

		if(!_validate_numeric($workout['sets'])){
			form_set_error($workout['sets'], t('Workout #'. $i .': Sets field must be a number'));	
		}

		if(!_validate_numeric($workout['reps'])){
			form_set_error($workout['reps'], t('Workout #'. $i .': Reps field must be a number'));	
		}

		$weight = $workout['weight'];

		if(!empty($weight['percentage'])){
			if(!_validate_numeric($weight['percentage'])){
				form_set_error($weight['percentage'], t('Workout #'. $i .': Weight percentage field must be a number'));	
			}

			if(!_validate_range($weight['percentage'], 1, 100)){
				form_set_error($weight['percentage'], t('Workout #'. $i .': Weight percentage field must be between 1 - 100'));	
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
      ),
      '#title' => t('Choose the style of conditioning workout'),
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
      '#description' => t('Number of rounds'),
      '#size' => 5,
      '#maxlength' => 10,
      '#states' => array(
				'visible' => array(
	    		':input[name="cond_workout[' . $k . '][condition_exercises][style]"]' => array(
	    			array('value' => 'for_time'),
	    			array('value' => 'intervals'),
	  			),
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

    for($i = 1; $i <= $form_state['num_condition'][$k]; $i++){
      $form['cond_workout'][$k]['condition_exercises'][$i] = array(
				'#type' => 'fieldset',
				'#title' => t('Movement #@num', array('@num' => $i)),
				'#collapsible' => TRUE,
				'#collapsed' => TRUE,
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

      $form['cond_workout'][$k]['condition_exercises'][$i]['reps'] = array(
				'#type' => 'textfield',
				'#title' => t('Number of Reps'),
				'#description' => t('Enter number of reps per set. (or MAX for max reps)'),
				'#size' => 3,
				'#maxlength' => 3,
				'#states' => array(
					'visible' => array(
						':input[name="cond_workout[' . $k . '][condition_exercises][style]"]' => array('!value' => 'intervals'),
					),
				),
      );

      $form['cond_workout'][$k]['condition_exercises'][$i]['interval_time'] = array(
				'#type' => 'textfield',
				'#title' => t('Movement Duration'),
				'#description' => t('Duration of Interval in Seconds'),
				'#size' => 4,
				'#maxlength' => 5,
				'#states' => array(
					'visible' => array(
						':input[name="cond_workout[' . $k . '][condition_exercises][style]"]' => array('value' => 'intervals'),
					),
				),
      );

      $form['cond_workout'][$k]['condition_exercises'][$i]['movement'] = array(
				'#type' => 'textfield',
				'#title' => t('Movement'),
				'#description' => t('Enter the movement'),
				'#size' => 20,
				'#maxlength' => 40,
      );

			$form['cond_workout'][$k]['condition_exercises'][$i]['weight_choice'] = array(
				'#type' => 'radios',
				'#options' => array(
					'weight' => t('Weight'),
					'distance' => t('Distance (box jumps, rowing, running, etc.)'),
				),
				'#title' => t('Do this movement for weight or distance?'),
			);

      $form['cond_workout'][$k]['condition_exercises'][$i]['weight'] = array(
				'#type' => 'fieldset',
				'#title' => t('Weight'),
				'#collapsible' => TRUE,
				'#collapsed' => TRUE,
				'#states' => array(
					'visible' => array(
						':input[name="cond_workout[' . $k . '][condition_exercises][' . $i . '][weight_choice]"]' => array('value' => 'weight'),
					),
				),
      );

      $form['cond_workout'][$k]['condition_exercises'][$i]['weight']['style'] = array(
				'#type' => 'radios',
				'#options' => array(
					'percentage' => t('Percentage'),
					'weight' => t('Weight'),
				),
				'#title' => t('Prescribe a percentage of max or a specific weight?'),
      );

      $form['cond_workout'][$k]['condition_exercises'][$i]['weight']['percentage'] = array(
				'#type' => 'textfield',
				'#description' => t('Enter prescribed percentage of max'),
				'#size' => 3,
				'#maxlength' => 5,
				'#states' => array(
					'visible' => array(
						':input[name="cond_workout[' . $k . '][condition_exercises][' . $i . '][weight][style]"]' => array('value' => 'percentage'),
					),
				),
      );

      $form['cond_workout'][$k]['condition_exercises'][$i]['weight']['men'] = array(
				'#type' => 'textfield',
				'#title' => t('Men'),
				'#description' => t('Enter prescribed weight for men'),
				'#size' => 3,
				'#maxlength' => 5,
				'#access' => in_array('coach', array_values($user->roles)),
				'#states' => array(
					'visible' => array(
						':input[name="cond_workout[' . $k . '][condition_exercises][' . $i . '][weight][style]"]' => array('value' => 'weight'),
					),
				),
      );

      $form['cond_workout'][$k]['condition_exercises'][$i]['weight']['women'] = array(
				'#type' => 'textfield',
				'#title' => t('Women'),
				'#description' => t('Enter prescribed weight for women'),
				'#size' => 3,
				'#maxlength' => 5,
				'#access' => in_array('coach', array_values($user->roles)),
				'#states' => array(
					'visible' => array(
						':input[name="cond_workout[' . $k . '][condition_exercises][' . $i . '][weight][style]"]' => array('value' => 'weight'),
					),
				),
      );
   
      $form['cond_workout'][$k]['condition_exercises'][$i]['weight']['self_prescribed'] = array(
				'#type' => 'textfield',
				'#description' => t('Enter prescribed weight'),
				'#size' => 3,
				'#maxlength' => 5,
				'#access' => in_array('athlete', array_values($user->roles)),
				'#states' => array(
					'visible' => array(
						':input[name="cond_workout[' . $k . '][condition_exercises][' . $i . '][weight][style]"]' => array('value' => 'weight'),
					),
				),
      );

      $form['cond_workout'][$k]['condition_exercises'][$i]['distance'] = array(
				'#type' => 'fieldset',
				'#title' => t('Distance'),
				'#collapsible' => TRUE,
				'#collapsed' => TRUE,
				'#states' => array(
					'visible' => array(
						':input[name="cond_workout[' . $k . '][condition_exercises][' . $i . '][weight_choice]"]' => array('value' => 'distance'),
					),
				),
      );

			$form['cond_workout'][$k]['condition_exercises'][$i]['distance']['units'] = array(
				'#type' => 'select',
				'#description' => t('Meters or inches'),
				'#options' => array(t('m'), t('in')),
			);

      $form['cond_workout'][$k]['condition_exercises'][$i]['distance']['men'] = array(
				'#type' => 'textfield',
				'#title' => t('Men'),
				'#description' => t('Enter Height/Distance for Men'),
				'#size' => 3,
				'#maxlength' => 5,
				'#access' => in_array('coach', array_values($user->roles)),
      );

			$form['cond_workout'][$k]['condition_exercises'][$i]['distance']['women'] = array(
				'#type' => 'textfield',
				'#title' => t('Women'),
				'#description' => t('Enter Height/Distance for Women'),
				'#size' => 3,
				'#maxlength' => 5,
				'#access' => in_array('coach', array_values($user->roles)),
      );

			$form['cond_workout'][$k]['condition_exercises'][$i]['distance']['self_prescribed'] = array(
				'#type' => 'textfield',
				'#description' => t('Enter Height/Distance'),
				'#size' => 3,
				'#maxlength' => 5,
				'#access' => in_array('athlete', array_values($user->roles)),
      );
    }

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

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Build Workout'),
    '#submit' => array('workout_builder_wbuild_page_two_submit'),
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

function workout_builder_wbuild_page_two_submit_validate($form, &$form_state){
	// validate errthang
}

function workout_builder_wbuild_page_two_submit($form, &$form_state){
  // do nothing for now
}


