<?php

function workout_builder_wbuilder($form, &$form_state){

  $form['#tree'] = TRUE;

  if(empty($form_state['num_strength'])){
    $form_state['num_strength'] = 1;
  }  

  $form['strength'] = array(
    '#type' => 'fieldset',
    '#title' => t('Strength Portion'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  for($i = 1; $i <= $form_state['num_strength']; $i++){
    $form['strength']['strength_exercises'][$i] = array(
      '#type' => 'fieldset',
      '#title' => t('Strength Exercise #@num', array('@num' => $i)),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );

    $form['strength']['strength_exercises'][$i]['set'] = array(
      '#type' => 'textfield',
      '#title' => t('Number of Sets'),
      '#description' => t('Enter number of sets'),
      '#size' => 2,
      '#maxlength' => 2,
    );

    $form['strength']['strength_exercises'][$i]['reps'] = array(
      '#type' => 'textfield',
      '#title' => t('Number of Reps'),
      '#description' => t('Enter number of reps per set'),
      '#size' => 3,
      '#maxlength' => 3,
    );

    $form['strength']['strength_exercises'][$i]['lift'] = array(
      '#type' => 'textfield',
      '#title' => t('Movement'),
      '#description' => t('Enter the movement'),
      '#size' => 20,
      '#maxlength' => 40,
    );

    $form['strength']['strength_exercises'][$i]['weight'] = array(
      '#type' => 'textfield',
      '#title' => t('Weight/Percentage'),
      '#description' => t('Enter prescribed weight/percentage of max'),
      '#size' => 3,
      '#maxlength' => 5,
    );
  }

  $form['strength']['clock'] = array(
    '#type' => 'textfield',
    '#title' => t('Clock'),
    '#description' => t('Enter timing in minutes'),
    '#size' => 5,
    '#maxlength' => 10,
  );

  $form['strength']['add_strength'] = array(
    '#type' => 'submit',
    '#value' => t('Add another strength movement'),
    '#submit' => array('workout_builder_wbuild_add_strength'),
  );

  if($form_state['num_strength'] > 1){
    $form['strength']['remove_strength'] = array(
      '#type' => 'submit',
      '#value' => t('Remove last strength movement'),
      '#submit' => array('workout_builder_wbuild_remove_strength'),
      '#limit_validate_errors' => array(),
    );
  }
/********************************************************************/

  if(empty($form_state['num_condition'])){
    $form_state['num_condition'] = 1;
  }  

  $form['condition'] = array(
    '#type' => 'fieldset',
    '#title' => t('Conditioning Portion'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['condition']['duration'] = array(
    '#type' => 'textfield',
    '#title' => t('Duration'),
    '#description' => t('Enter timing in minutes'),
    '#size' => 5,
    '#maxlength' => 10,
  );

  for($i = 1; $i <= $form_state['num_condition']; $i++){
    $form['condition']['condition_exercises'][$i] = array(
      '#type' => 'fieldset',
      '#title' => t('Conditioning Exercise #@num', array('@num' => $i)),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    );

    $form['condition']['condition_exercises'][$i]['reps'] = array(
      '#type' => 'textfield',
      '#title' => t('Number of Reps'),
      '#description' => t('Enter number of reps per set'),
      '#size' => 3,
      '#maxlength' => 3,
    );

    $form['condition']['condition_exercises'][$i]['movement'] = array(
      '#type' => 'textfield',
      '#title' => t('Movement'),
      '#description' => t('Enter the movement'),
      '#size' => 20,
      '#maxlength' => 40,
    );

    $form['condition']['condition_exercises'][$i]['weight'] = array(
      '#type' => 'textfield',
      '#title' => t('Weight/Percentage'),
      '#description' => t('Enter prescribed weight'),
      '#size' => 3,
      '#maxlength' => 5,
    );
  }

  $form['condition']['cutoff'] = array(
    '#type' => 'textfield',
    '#title' => t('Time Cutoff'),
    '#description' => t('Minutes'),
    '#size' => 5,
    '#maxlength' => 10,
  );
/*
  $form['back'] = array(
    '#type' => 'submit',
    '#value' => t('Strength'),
    '#submit' => array('workout_builder_wbuild_page_two_back'),
    '#limit_validation_errors' => array(),
  );
*/
  $form['condition']['add_condition'] = array(
    '#type' => 'submit',
    '#value' => t('Add another movement'),
    '#submit' => array('workout_builder_wbuild_add_condition'),
  );

  if($form_state['num_condition'] > 1){
    $form['condition']['remove_condition'] = array(
      '#type' => 'submit',
      '#value' => t('Remove last movement'),
      '#submit' => array('workout_builder_wbuild_remove_condition'),
      '#limit_validate_errors' => array(),
    );
  }
 
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Build Workout'),
    '#submit' => array('workout_builder_wbuild_page_two_submit'),
  );

/******************************************************************/

  return $form;
}

function workout_builder_wbuild_add_strength($form, &$form_state){
  $form_state['num_strength']++;
  $form_state['rebuild'] = TRUE;
}

function workout_builder_wbuild_remove_strength($form, &$form_state){
  if($form_state['num_strength'] > 1){
    $form_state['num_strength']--;
  }
  $form_state['rebuild'] = TRUE;
}

function workout_builder_wbuild_add_condition($form, &$form_state){
  $form_state['num_condition']++;
  $form_state['rebuild'] = TRUE;
}

function workout_builder_wbuild_remove_condition($form, &$form_state){
  if($form_state['num_condition'] > 1){
    $form_state['num_condition']--;
  }
  $form_state['rebuild'] = TRUE;
}

function workout_builder_wbuild_page_two_submit($form, &$form_state){
  // do nothing for now
}
