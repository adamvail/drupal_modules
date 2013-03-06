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
  $num_workouts = $form_state['num_strength_wo'];
  if(empty($form_state['num_strength'][$num_workouts])){
    $form_state['num_strength'][$num_workouts] = 1;
  }  

	for($k = 1; $k <= $form_state['num_strength_wo']; $k++){

		$form['strength_wo'][$k] = array(
			'#type' => 'fieldset',
      '#title' => t('Workout #@num', array('@num' => $k)),
			'#collapsible' => TRUE,
			'#collapsed' => TRUE,
		);

		$form['strength_wo'][$k]['clock'] = array(
			'#type' => 'textfield',
			'#title' => t('Clock'),
			'#description' => t('Time per set in minutes'),
			'#size' => 5,
			'#maxlength' => 10,
		);

		for($i = 1; $i <= $form_state['num_strength'][$k]; $i++){
			$form['strength_wo'][$k]['strength_exercises'][$i] = array(
				'#type' => 'fieldset',
				'#title' => t('Movement #@num', array('@num' => $i)),
				'#collapsible' => TRUE,
				'#collapsed' => TRUE,
			);

			$form['strength_wo'][$k]['strength_exercises'][$i]['set'] = array(
				'#type' => 'textfield',
				'#title' => t('Number of Sets'),
				'#description' => t('Enter number of sets'),
				'#size' => 2,
				'#maxlength' => 2,
			);

			$form['strength_wo'][$k]['strength_exercises'][$i]['reps'] = array(
				'#type' => 'textfield',
				'#title' => t('Number of Reps'),
				'#description' => t('Enter number of reps per set. (or MAX for max reps)'),
				'#size' => 3,
				'#maxlength' => 3,
			);

			$form['strength_wo'][$k]['strength_exercises'][$i]['lift'] = array(
				'#type' => 'textfield',
				'#title' => t('Movement'),
				'#description' => t('Enter the movement'),
				'#size' => 20,
				'#maxlength' => 40,
			);

			$form['strength_wo'][$k]['strength_exercises'][$i]['weight'] = array(
				'#type' => 'fieldset',
				'#title' => t('Weight'),
				'#collapsible' => TRUE,
				'#collapsed' => FALSE,
			);

			$form['strength_wo'][$k]['strength_exercises'][$i]['weight']['style'] = array(
				'#type' => 'radios',
	//      '#options' => drupal_map_assoc(array(t('Percentage'), t('Weight'))),
				'#options' => array(
					'percentage' => t('Percentage'),
					'weight' => t('Weight'),
				),
				'#title' => t('Prescribe a percentage of max or a specific weight?'),
			);

			$form['strength_wo'][$k]['strength_exercises'][$i]['weight']['percentage'] = array(
				'#type' => 'textfield',
				'#description' => t('Enter prescribed percentage of max'),
				'#size' => 3,
				'#maxlength' => 5,
				'#states' => array(
					'visible' => array(
						':input[name="strength_wo[' . $k . '][strength_exercises][' . $i . '][weight][style]"]' => array('value' => 'percentage'),
					),
				),
			);

			$form['strength_wo'][$k]['strength_exercises'][$i]['weight']['men'] = array(
				'#type' => 'textfield',
				'#title' => t('Men'),
				'#description' => t('Enter prescribed weight for men'),
				'#size' => 3,
				'#maxlength' => 5,
				'#access' => in_array('coach', array_values($user->roles)),
				'#states' => array(
					'visible' => array(
						':input[name="strength_wo[' . $k . '][strength_exercises][' . $i . '][weight][style]"]' => array('value' => 'weight'),
					),
				),
			);

			$form['strength_wo'][$k]['strength_exercises'][$i]['weight']['women'] = array(
				'#type' => 'textfield',
				'#title' => t('Women'),
				'#description' => t('Enter prescribed weight for women'),
				'#size' => 3,
				'#maxlength' => 5,
				'#access' => in_array('coach', array_values($user->roles)),
				'#states' => array(
					'visible' => array(
						':input[name="strength_wo[' . $k . '][strength_exercises][' . $i . '][weight][style]"]' => array('value' => 'weight'),
					),
				),
			);

			$form['strength_wo'][$k]['strength_exercises'][$i]['weight']['self_prescribed'] = array(
				'#type' => 'textfield',
				'#description' => t('Enter prescribed weight.'),
				'#size' => 3,
				'#maxlength' => 5,
				'#access' => in_array('athlete', array_values($user->roles)),
				'#states' => array(
					'visible' => array(
						':input[name="strength_wo[' . $k . '][strength_exercises][' . $i . '][weight][style]"]' => array('value' => 'weight'),
					),
				),
			);
		}

		$form['strength_wo'][$k]['add_strength'] = array(
			'#type' => 'submit',
			'#name' => $k,
			'#value' => t('Add another movement'),
			'#submit' => array('workout_builder_wbuild_add_strength'),
		);

		if($form_state['num_strength'][$k] > 1){
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
  $form_state['num_strength'][$index]++;
  $form_state['rebuild'] = TRUE;
}

function workout_builder_wbuild_remove_strength($form, &$form_state){
	$index = $form_state['triggering_element']['#name'];
  if($form_state['num_strength'][$index] > 1){
    $form_state['num_strength'][$index]--;
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
		$form_state['num_strength'][$index] = 1;
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
  // Do nothing for now
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

      $form['cond_workout'][$k]['condition_exercises'][$i]['weight'] = array(
				'#type' => 'fieldset',
				'#title' => t('Weight'),
				'#collapsible' => TRUE,
				'#collapsed' => FALSE,
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

function workout_builder_wbuild_page_two_submit($form, &$form_state){
  // do nothing for now
}


