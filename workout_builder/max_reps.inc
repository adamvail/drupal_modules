<?php


/**
 * Define a form.
 */
function max_reps() {
	global $user;
	$form['#tree'] = TRUE;
	$form['sets'] = array(
		'#title' => t('Sets'),
		'#type' => 'textfield',
		'#size' => 5,
		'#maxlength' => 5,
		'#description' => t('Enter sets'),
	);
	$form['reps'] = array(
		'#title' => t('Reps'),
		'#type' => 'textfield',
		'#size' => 5,
		'#maxlength' => 5,
		'#description' => t('Enter reps'),
	);
	$form['exercise'] = array(
		'#title' => t('Movement'),
		'#type' => 'textfield',
		'#size' => 5,
		'#maxlength' => 5,
		'#description' => t('Enter exercise'),
	);
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Submit')
	);
	return $form;
}