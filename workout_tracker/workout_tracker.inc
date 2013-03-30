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

	$gym = db_query('SELECT gym FROM {workout_gym_affiliation} where uid=:uid', array(':uid' => $user->uid))->fetchField();
	$top_strength = db_query("SELECT wid, date FROM {workout_builder_strength} WHERE gym=:gym ORDER BY wid DESC LIMIT 7", array(':gym' => $gym));
	$top_conditioning = db_query("SELECT wid, date FROM {workout_builder_conditioning} WHERE gym=:gym ORDER BY wid DESC LIMIT 7", array(':gym' => $gym));

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
/*
	$week = array_slice($top_ten, 0, 7);
	drupal_set_message('<pre>' . print_r($week, TRUE) . '</pre>');
	drupal_set_message('<pre>' . print_r($top_ten, TRUE) . '</pre>');
*/
	$form['description'] = array(
		'#type' => 'item',
//		'#title' => t('Pick a Workout'), 
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
	$form_state['wid'] = $wid;

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
		$all_lifts = array();
		$set_header = "";	
		foreach($strength_data as $workout){
			$form_state['strength_exists'] = TRUE;
			// make the header string
			$movement_str = "";
			if(isset($workout[0]->wgt_style) && $workout[0]->wgt_style == 'percentage' && (sizeof(explode("-", $workout[0]->wgt_percentage)) > 1)){
				$percentages = explode("-", $workout[0]->wgt_percentage);
				for($r = 1; $r <= sizeof($percentages); $r++){
					$percentage_scheme[$r] = $percentages[$r - 1];
				}
				
				if(strpos($workout[0]->rep_scheme, "-") !== FALSE){
					$scheme = explode("-", $workout[0]->rep_scheme);
					for($r = 1; $r <= sizeof($scheme); $r++){
						$rep_scheme[$r] = $scheme[$r - 1];
					}
				}			
				elseif(strpos($workout[0]->rep_scheme, "x") !== FALSE){
					$scheme = explode("x", $workout[0]->rep_scheme);
					for($r = 1; $r <= intval(trim($scheme[0])); $r++){
						$rep_scheme[$r] = $scheme[1];
					}
				}
					//drupal_set_message('<pre>' . print_r($rep_scheme, TRUE) . '</pre>');

				// try to coalesce sets together
				for($p = 1; $p <= sizeof($percentage_scheme); $p++){
					//drupal_set_message('<pre>' . print_r($p, TRUE) . '</pre>');
					if($p == sizeof($percentage_scheme)){
						// This hasn't been coalesced and won't be since it's the last one
						// so print it out
						$movement_str .= $rep_scheme[$p] . " @ " . $percentage_scheme[$p] . "%, ";
					}

					for($k = $p + 1; $k <= sizeof($percentage_scheme); $k++){
						if($percentage_scheme[$p] != $percentage_scheme[$k] || 
								($percentage_scheme[$p] == $percentage_scheme[$k] && $k == sizeof($percentage_scheme)) || 
								($rep_scheme[$p] != $rep_scheme[$k])){
							if($k == $p + 1 && ($rep_scheme[$p] != $rep_scheme[$k]  || $percentage_scheme[$p] != $percentage_scheme[$k])){
								$movement_str .= $rep_scheme[$p] . " @ " . $percentage_scheme[$p] . "%, ";
								break;
							}
							// This means we found some that are the same
							$num_sets = $k - $p;
							$movement_str .= $num_sets . "x" . $rep_scheme[$p] . " @ " . $percentage_scheme[$p] . "%, ";

							$p = $k - 1;
							break;
						}
					}
				}

				$movement_pretty = implode(' ', array_map('ucfirst', explode(' ', $workout[0]->movement)));
				$movement_pretty = implode('-', array_map('ucfirst', explode('-', $movement_pretty)));
				$movement_pretty = implode('[', array_map('ucfirst', explode('[', $movement_pretty)));
				$movement_pretty = implode(']', array_map('ucfirst', explode(']', $movement_pretty)));
				$movement_pretty = implode('+', array_map('ucfirst', explode('+', $movement_pretty)));
				$movement_str .= $movement_pretty;
			}

			else{
				if(!empty($workout[0]->rep_scheme)){
					$movement_str .= $workout[0]->rep_scheme . ',';
				}
				$movement_pretty = implode(' ', array_map('ucfirst', explode(' ', $workout[0]->movement)));
				$movement_pretty = implode('-', array_map('ucfirst', explode('-', $movement_pretty)));
				$movement_pretty = implode('[', array_map('ucfirst', explode('[', $movement_pretty)));
				$movement_pretty = implode(']', array_map('ucfirst', explode(']', $movement_pretty)));
				$movement_pretty = implode('+', array_map('ucfirst', explode('+', $movement_pretty)));
				for($i = 0; isset($workout[$i]); $i++){
					if($i == 0){
							$movement_str = $movement_str . ' ' . $movement_pretty;
							$set_header .= $movement_pretty;
					}
					else{
							$movement_str = $movement_str . ' + ' . $movement_pretty;
							$set_header .= ' + ' . $movement_pretty;
					}
				}

				if(isset($workout[0]->wgt_style)){
					switch($workout[0]->wgt_style){
						case 'ahap':
							$movement_str = $movement_str . ', AHAP';
							$set_header .= ', AHAP';
							break;
						case 'percentage': 
							$movement_str = $movement_str . ' at ' . $workout[0]->wgt_percentage . '%';
							$set_header .= ' at ' . $workout[0]->wgt_percentage . '%';
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
								$set_header .= ' at ' . $workout[0]->wgt_self_rx . ' ' . $units;
								break;
							}
							else{
								$movement_str = $movement_str . ' at ' . $workout[0]->wgt_men . '/' . $workout[0]->wgt_women . ' ' . $units;
								$set_header .= ' at ' . $workout[0]->wgt_men . '/' . $workout[0]->wgt_women . ' ' . $units;
								break;	
							}
					}
				}
			}

			if(isset($workout[0]->clock) && !empty($workout[0]->clock)){
				if(strpos($workout[0]->clock, ":") !== FALSE){
					$movement_str = $movement_str . ' on a ' . $workout[0]->clock . ' clock';
				}
				else {
					$movement_str = $movement_str . ' on a ' . $workout[0]->clock . ':00 clock';
				}
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
				//'#markup' => '<p>' . build_strength_text($wid) . '</p>',
			);	

			if(isset($strength_notes[$workout[0]->strength_id]->notes)){
				$form['strength'][$workout[0]->strength_id]['strength_notes'] = array(
					'#markup' => '<p>' . $strength_notes[$workout[0]->strength_id]->notes . '</p>',
				);	
			}
	
			//drupal_set_message('<pre>' . print_r($movement_str, TRUE) . '</pre>');

			// constuct boxes based on sets for weight/count input
			$rep_scheme = $workout[0]->rep_scheme;
			$pivot = strpos($rep_scheme, 'x');
			if($pivot !== FALSE && !empty($rep_scheme)){
				// set it up by 3x4
				$sets = trim(substr($rep_scheme, 0, $pivot));
				$rep = trim(substr($rep_scheme, $pivot+1));
				for($j=1; $j <= $sets; $j++){
					$reps[] = $rep;
				}
			}
			elseif($pivot === FALSE && !empty($rep_scheme)){ 	// triple equals is not a typo, if the str position is 0, 0 == FALSE will eval to true, use === instead
				$pivot = strpos($rep_scheme, '-');
				if($pivot === FALSE){
					drupal_set_message('<pre>' . print_r("UNKNOWN REP SCHEME STYLE, VALIDATE FOR THIS IN BUILDER", TRUE) . '</pre>');
					return;
				}
				// Scheme is 5-3-2-2-1 style
				$reps = explode("-", $rep_scheme);
				$sets = count($reps);
			}
			else {
				$sets = 1;
				$reps = 0;
			}

			// This handles specifying failures with complex movements
			$combo = array();
			if(substr_count($workout[0]->movement, "[") > 1){
				// TODO check for this in the builder and disallow it for now
				$complex_strength = FALSE;
			}
			elseif(substr_count($workout[0]->movement, "[") == 1){
				$complex_strength = TRUE;
				$open_brack_pos = strpos($workout[0]->movement, "[");
				$closing_brack_pos = strpos($workout[0]->movement, "]");
				$lift_combo = substr($workout[0]->movement, $open_brack_pos+1, $closing_brack_pos - 1 - $open_brack_pos);
				$reverse_start = (strlen($workout[0]->movement) - $open_brack_pos) * -1;
				$prev_split = strrpos($workout[0]->movement, "+", $reverse_start);
				$lift_reps = 1;
				
				if($prev_split === FALSE || $prev_split > $open_brack_pos){
					// find reps from beginning of string
					$lift_reps = substr($workout[0]->movement, 0, $open_brack_pos);
					$lift_minus_combo = substr_replace($workout[0]->movement, "", 0, $closing_brack_pos+1);
					// -------------------------------------------
					$individual_lifts = explode("+", $lift_combo);
					foreach($individual_lifts as $indv_lift){
						if(trim($indv_lift) != ""){
							$combo[trim($indv_lift)] = $lift_reps;
						}
					}

					// grab the rest of the lifts and give a rep count of 1
					$remaining_lifts = explode("+", $lift_minus_combo);
					for($r = 0; $r < sizeof($remaining_lifts); $r++){
						if(trim($remaining_lifts[$r]) != ""){
							$combo[trim($remaining_lifts[$r])] = 1;
						}
					}
					//----------------------------------------------
				}
				else {
					// find reps from position of '+'
					$lift_reps = substr($workout[0]->movement, $prev_split + 1, $open_brack_pos - 1 - $prev_split);
					$lift_minus_combo = substr_replace($workout[0]->movement, "", $prev_split+1, $closing_brack_pos - $prev_split);

					//----------------------------------------------------
					// grab the rest of the lifts and give a rep count of 1
					$remaining_lifts = explode("+", $lift_minus_combo);
					for($r = 0; $r < sizeof($remaining_lifts); $r++){
						if(trim($remaining_lifts[$r]) != ""){
							$combo[trim($remaining_lifts[$r])] = 1;
						}
					}
					// split up complex move into individual moves to specify which was failed on	
					$individual_lifts = explode("+", $lift_combo);
					foreach($individual_lifts as $indv_lift){
						if(trim($indv_lift) != ""){
							$combo[trim($indv_lift)] = $lift_reps;
						}
					}
					//-------------------------------------------------------

				}
				
				foreach(array_keys($combo) as $key){
					$lift_reps = $combo[$key];
					$rep_values = array();
					for($rep = 1; $rep <= $lift_reps; $rep++){
						$rep_values[$rep] = $rep;
					}
					$combo[$key] = $rep_values;
					$all_lifts[$workout[0]->strength_id][$key] = $rep_values;
				}
			}
			else {
				if(strpos($workout[0]->movement, "+") !== FALSE){
					$complex_strength = TRUE;
					// complex movement with each movement getting a single rep
					$lifts = explode("+", $workout[0]->movement);
					for($l = 0; $l < sizeof($lifts); $l++){
						if(trim($lifts[$l]) != ""){
							$rep[1] = 1;
							$combo[trim($lifts[$l])] = $rep;
							$all_lifts[$workout[0]->strength_id][trim($lifts[$l])] = $rep;
						}
					}
				}
				else{
					$combo[$workout[0]->movement] = 1;
					$all_lifts[$workout[0]->strength_id][$workout[0]->movement] = 1;
					$complex_strength = FALSE;
				}
			}
			//drupal_set_message('<pre>' . print_r($combo, TRUE) . '</pre>');


			$percentage_scheme = explode("-", $workout[0]->wgt_percentage);
			// Stqndard form items for all movements, regardless of complexity
			for($k = 1; $k <= $sets; $k++){
				$reps_array = array();
				for($j = 1; $j <= $reps[$k-1]; $j++){
					$reps_array[$j] = $j;
				}

				if(sizeof($percentage_scheme) > 1){
					if($complex_strength == FALSE){
						// if there is only one movement, then populate the default with the % from the PR
						global $user;
						$lift = $workout[0]->movement;
						$pr = db_query('SELECT pr from {workout_pr} where athlete_uid=:auid and movement=:movement', array(':auid' => $user->uid, ':movement' => $lift))->fetchField();
						$prescribed = $pr * ($percentage_scheme[$k-1] / 100);
						if(sizeof($reps_array == 1)){
							$form['strength'][$workout[0]->strength_id][$k]['work'] = array(
								'#type' => 'textfield',
								'#title' => t('1 Rep: @movement @ @percentage%', array('@movement' => $lift, '@percentage' => $percentage_scheme[$k-1])),
								'#size' => 10,
								'#maxlength' => 20,
								'#default_value' => ($prescribed > 0) ? $prescribed : '',
							);
						}
						else {
							$form['strength'][$workout[0]->strength_id][$k]['work'] = array(
								'#type' => 'textfield',
								'#title' => t('@num_reps Reps: @movement @ @percentage%', array('@num_reps' => sizeof($reps_array),'@movement' => $lift, '@percentage' => $percentage_scheme[$k-1])),
								'#size' => 10,
								'#maxlength' => 20,
								'#default_value' => ($prescribed > 0) ? $prescribed : '',
							);
						}
					}
					else {
						if(sizeof($reps_array == 1)){
							$form['strength'][$workout[0]->strength_id][$k]['work'] = array(
								'#type' => 'textfield',
								'#title' => t('1 Rep: @movement @ @percentage%', array('@movement' => $lift, '@percentage' => $percentage_scheme[$k-1])),
								'#size' => 10,
								'#maxlength' => 20,
							);
						}
						else {
							$form['strength'][$workout[0]->strength_id][$k]['work'] = array(
								'#type' => 'textfield',
								'#title' => t('@num_reps Reps: @movement @ @percentage%', array('@num_reps' => sizeof($reps_array),'@movement' => $lift, '@percentage' => $percentage_scheme[$k-1])),
								//'#title' => t('Set #@num Weight/Count: @percentage%', array('@num' => $k, '@percentage' => $percentage_scheme[$k-1])),
								'#size' => 10,
								'#maxlength' => 20,
							);
						}
					}
				}
				else {	
					if(sizeof($reps_array) == 0){
						$form['strength'][$workout[0]->strength_id][$k]['work'] = array(
							'#type' => 'textfield',
							'#title' => t('@movement', array('@movement' => $set_header)),
							'#size' => 10,
							'#maxlength' => 20,
						);
					}
					elseif(sizeof($reps_array) == 1) {
						$form['strength'][$workout[0]->strength_id][$k]['work'] = array(
							'#type' => 'textfield',
							'#title' => t('1 Rep: @movement', array('@movement' => $set_header)),
							'#size' => 10,
							'#maxlength' => 20,
						);
					}
					else {
						$form['strength'][$workout[0]->strength_id][$k]['work'] = array(
							'#type' => 'textfield',
							'#title' => t('@num_reps Reps: @movement', array('@num_reps' => sizeof($reps_array), '@movement' => $set_header)),
							//'#title' => t('Set #@num Weight/Count', array('@num' => $k)),
							'#size' => 10,
							'#maxlength' => 20,
						);
					}
				}				
	
				if($reps > 0){
					$form['strength'][$workout[0]->strength_id][$k]['fail'] = array(
						'#type' => 'checkbox',
						'#title' => t('Fail on a rep?'),
					);
						
					$form['strength'][$workout[0]->strength_id][$k]['fail_rep'] = array(
						'#type' => 'radios',
						'#title' => t('Failed on rep:'),
						'#options' => $reps_array,
						'#default_value' => key($reps_array),
						'#states' => array(
							'visible' => array(
								':input[name="strength[' . $workout[0]->strength_id . '][' . $k . '][fail]"]' => array('checked' => TRUE),
							),
						),
					);
				}

				// form items for failures of complex strength movements
				if($complex_strength == TRUE){	
					$lift_keys = array();
					foreach(array_keys($combo) as $combo_key){
						$lift_keys[$combo_key] = $combo_key;
					}			
		
						// use this info to build form elements
						$form['strength'][$workout[0]->strength_id][$k]['fail_lift'] = array(
							'#type' => 'radios',
							'#title' => t('Failed on move:'),
							'#options' => $lift_keys,
							'#default_value' => key($lift_keys),
							'#states' => array(
								'visible' => array(
									':input[name="strength[' . $workout[0]->strength_id . '][' . $k . '][fail]"]' => array('checked' => TRUE),
								),
							),
						);
						// This is supposed to build form elements to specify on which movement rep you failed
						// for a complex movement....but it doesn't work...i'll come back to it
				/*	
						foreach($lift_keys as $l){
						//drupal_set_message('<pre>' . print_r($combo[$l], TRUE) . '</pre>');

						$form['strength'][$workout[0]->strength_id][$k]['fail_lift'][$l] = array(
								'#type' => 'radios',
								'#title' => t('Failed on movement rep:'),
								'#options' => $combo[$l],
								//'#default_value' => key($combo[$l]),
								'#states' => array(
									'visible' => array(
										':input[name="strength[' . $workout[0]->strength_id . '][' . $k . '][fail]"]' => array('checked' => TRUE),
										':input[name="strength[' . $workout[0]->strength_id . '][' . $k . '][fail_lift]"]' => array('value' => $l),
									),
								),
							);

						}
*/
				}
			}
		}	
			$form_state['lifts'] = $all_lifts;
			$form_state['wid'] = $wid;
	}

	if(!empty($conditioning) && isset($conditioning)){
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
			$form_state['conditioning_exists'] = TRUE;
			// $workout is an array of movements all with the same conditioning_id

			$fieldset_title = '';
			if($workout[0]->style == 'amrap'){
				$movement_str = $workout[0]->duration . " minute AMRAP:<br>";
				$fieldset_title = substr($movement_str, 0, strlen($movement_str) - 5);
			}
			elseif($workout[0]->style == 'for_time'){
				$movement_str = $workout[0]->rounds . " Rounds For Time";
				if(!empty($workout[0]->cutoff)){
					$movement_str = $movement_str . ", " . $workout[0]->cutoff . " minute cutoff:<br>";	
				}
				else{
					$movement_str = $movement_str . ":<br>";
				}
				$fieldset_title = substr($movement_str, 0, strlen($movement_str) - 5);
			}
			elseif($workout[0]->style == 'intervals' && $workout[0]->rounds > 1){
				$movement_str = $workout[0]->rounds . " Rounds of:<br>";
				$fieldset_title = 'Intervals';
			}
			
			// replace pipes with html new lines
			$movement_str = $movement_str . str_replace("&", "<br>", $workout[0]->workout);

			// build the workout string
/*			for($i = 0; isset($workout[$i]); $i++){
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
*/
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
						'#maxlength' => 5,
					); 
				}

				if($workout[0]->style == 'intervals'){
					$rounds = $workout[0]->rounds;
					$work = $workout[0]->workout;

					$intervals = explode("<br>", $work);
					$num_moves = sizeof($intervals);
					$form_state['interval_rounds'] = $rounds;
					$form_state['interval_num_moves'] = $num_moves;
					//drupal_set_message('<pre>' . print_r($num_intervals, TRUE) . '</pre>');
				
					for($interval = 1; $interval <= $rounds; $interval++){
						$form['conditioning'][$workout[0]->conditioning_id]['intervals'][$interval] = array(
							'#type' => 'fieldset',
							'#title' => t('Round #@int_num', array('@int_num' => $interval)),
							'#collapsible' => TRUE,
							'#collapsed' => TRUE,
						);
						for($move = 1; $move <= $num_moves; $move++){
							$form['conditioning'][$workout[0]->conditioning_id]['intervals'][$interval][$move] = array(
								'#type' => 'textfield',
								'#title' => t($intervals[$move - 1]),
								'#size' => 5,
								'#maxlength' => 15,
							);					
						}
					}
				}
			}
		}

	$form['save'] = array(
	'#type' => 'submit',
	'#value' => t('Save'),
	'#submit' => array('workout_tracker_save_submit'),
	);

	return $form;
}

function convert_timestamp($timestamp){
	$format = 'l, n/j/Y';
	return date($format, $timestamp);
}

function workout_tracker_save_submit($form, &$form_state){
	if(isset($form_state['strength_exists'])){
		_save_strength_results($form_state);
	}
	if(isset($form_state['conditioning_exists']) && $form_state['conditioning_exists'] == TRUE){
		_save_conditioning_results($form_state);
	}
}

function _save_strength_results(&$form_state){
	global $user;
	
	$strength_table = 'workout_tracker_strength';
	$strength_table_notes = 'workout_tracker_strength_notes';

	// figure out how many times this workout has been done before
	$num_performed = db_query('SELECT MAX(num_performed) from {workout_tracker_strength} where wid=:wid', array(':wid' => $form_state['wid']))->fetchField();

	if(empty($num_performed)){
		// workout has never been done
		$num_performed = 1;
	}
	else{
		// workout has been done, so increase the num count
		$num_performed += 1;
	}

	$all_lifts = $form_state['lifts'];
	$lift_results = array();

	for($i = 1; isset($form_state['input']['strength'][$i]); $i++){
		$workout = $form_state['input']['strength'][$i];
		$lifts = array_keys($all_lifts[$i]);
		for($s = 1; isset($workout[$s]); $s++){
			$set = $workout[$s];

			$record = new stdClass();
			$record->wid = $form_state['wid'];	
			$record->sid = $i;
			$record->athlete_uid = $user->uid;
			$record->num_performed = $num_performed;
			$record->set = $s;
			if(isset($set['work'])){
				$record->work = $set['work'];
			}
			if(isset($set['fail'])){
				$record->fail = $set['fail_rep'];
				if(sizeof($lifts) == 1){
					$set['fail_lift'] = $lifts[0];
				}
				if(isset($set['fail_lift'])){
					$record->fail_movement = $set['fail_lift'];
					foreach($lifts as $lift){
						if($lift != $set['fail_lift'] || $set['fail_rep'] > 1){
							$lift_results[$lift] = $set['work'];
						}
						else{
							break;
						}
					}
				}
			}
			else{
				// didn't fail, so map weights to lifts
				foreach($lifts as $lift){
					$lift_results[$lift] = $set['work'];
				}
			}
	
			drupal_write_record($strength_table, $record);
		}
	}
	foreach(array_keys($lift_results) as $l){
		//drupal_set_message('<pre>' . print_r($l, TRUE) . '</pre>');
		check_pr($l, $lift_results[$l]);
	}
}

function _save_conditioning_results(&$form_state){
	global $user;
	$conditioning_table = 'workout_tracker_conditioning';
	$conditioning_notes_table = 'workout_tracker_conditioning_notes';

	//drupal_set_message('<pre>' . print_r($form_state['wid'], TRUE) . '</pre>');
	$num_performed = db_query('SELECT MAX(num_performed) from {workout_tracker_conditioning} where wid=:wid', array(':wid' => $form_state['wid']))->fetchField();

	if(empty($num_performed)){
		// workout has never been done
		$num_performed = 1;
	}
	else{
		// workout has been done, so increase the num count
		$num_performed += 1;
	}

	for($i = 1; isset($form_state['input']['conditioning'][$i]); $i++){
		$workout = $form_state['input']['conditioning'][$i];
		//$movements = db_query('SELECT workout FROM {workout_builder_conditioning} where wid=:wid and conditioning_id=:cid', array(':wid' => $form_state['wid'], ':cid' => $i))->fetchField();
		//drupal_set_message('<pre>' . print_r($movements, TRUE) . '</pre>');
		$record = new stdClass();
		$record->wid = $form_state['wid'];
		$record->cid = $i;
		$record->athlete_uid = $user->uid;
		$record->num_performed = $num_performed;	

		if(isset($workout['amrap_rounds']) || isset($workout['amrap_reps'])){
			if(isset($workout['amrap_rounds'])){
				$record->amrap_rounds = $workout['amrap_rounds'];
			}
			else{
				$record->amrap_round = 0;
			}

			if(isset($workout['amrap_reps'])){
				$record->amrap_reps = $workout['amrap_reps'];
			}
			else {
				$record->amrap_reps = 0;
			}
		}
		
		if(isset($workout['for_time'])){
			$record->time = $workout['for_time'];
		}
		// TODO save interval numbers. Coalesce them all together?

		if(isset($workout['intervals'])){
			$results = '';
			for($interval = 1; $interval <= $form_state['interval_rounds']; $interval++){
				for($move = 1; $move <= $form_state['interval_num_moves']; $move++){		
					($move > 1) ? $results .= ',' . $workout['intervals'][$interval][$move] : $results .= $workout['intervals'][$interval][$move];
				}
				if($interval < $form_state['interval_rounds']){
					$results .= '-';
				}
			}
			$record->interval_reps = $results;
	//		drupal_set_message('<pre>' . print_r($record->interval_reps, TRUE) . '</pre>');
		}
		drupal_write_record($conditioning_table, $record);
	}
}

function check_pr($lift, $weight){
	global $user;
	drupal_set_message('<pre>' . print_r($lift . ':' . $weight, TRUE) . '</pre>');
	$pr_table = 'workout_pr';
	$movement = strtolower($lift);

	$cur_pr = db_query('SELECT pr FROM {workout_pr} where athlete_uid=:auid AND movement=:movement', array(':auid' => $user->uid, ':movement' => $movement))->fetchField();

	$record = new stdClass();
	$record->athlete_uid = $user->uid;
	$record->date = time(); 

	if(!empty($cur_pr)){
		if($weight > $cur_pr){
			$result = db_query('UPDATE {workout_pr} SET pr=:pr WHERE athlete_uid=:auid and movement=:movement', array(':pr' => $weight, ':auid' => $user->uid, ':movement' => $movement));
			if($result != 1){
				drupal_set_message('<pre>' . print_r("RESULT from PR UPDATE was NOT a single row!", TRUE) . '</pre>');
			}	
		}
	}
	else {
		// this lift isn't already in the database
		$record->movement = $movement;
		$record->pr = $weight;
		drupal_write_record($pr_table, $record);
	}
}

