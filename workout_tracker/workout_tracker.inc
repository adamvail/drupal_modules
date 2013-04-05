<?php

function workout_tracker($form, &$form_state){
	global $user;

	$form['#tree'] = TRUE;
	$cur_time = time();

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

	if(isset($gym) && !empty($gym)){
		// don't grab everyone from the gym's workouts, only grab works from this gym where the coach created it.
		$coaches = db_query('SELECT uid from {workout_gym_affiliation} where gym=:gym and role=:role', array(':gym' => $gym, ':role' => 'coach'));
		$creator_ids = array();
		foreach($coaches as $c){
			$creator_ids[] = $c->uid;
		}
		$creator_ids[] = $user->uid;

		$top_strength = db_query("SELECT wid, date, creator_id FROM {workout_builder_strength} WHERE date<:date AND creator_id IN (:creator) ORDER BY wid DESC LIMIT 7", array(':date' => time(), ':creator' => $creator_ids));
		$top_conditioning = db_query("SELECT wid, date, creator_id FROM {workout_builder_conditioning} WHERE date<:date AND creator_id IN (:creator) ORDER BY wid DESC LIMIT 7", array(':date' => time(), ':creator' => $creator_ids));
	}
	else {
		$top_strength = db_query("SELECT wid, date, creator_id FROM {workout_builder_strength} WHERE date<:date AND creator_id=:creator ORDER BY wid DESC LIMIT 7", array(':date' => time(), ':creator' => $user->uid));
		$top_conditioning = db_query("SELECT wid, date, creator_id FROM {workout_builder_conditioning} WHERE date<:date AND creator_id=:creator ORDER BY wid DESC LIMIT 7", array(':date' => time(), ':creator' => $user->uid));
	}

	if(empty($top_strength) && empty($top_conditioning)){
		$form['header'] = array(
			'#markup' => '<p>There are no workouts built, please use the Workout Builder to create a workout</p>',
		);	
		return $form;
	}

	$creators = array();
	// Take the last 10 workouts from both strength and conditioning
	// and make them one unified array to populate radio buttons
	foreach($top_strength as $ts){
		if($ts->creator_id == $user->uid){
			$creator = "You";
		}
		else{
			$creator = db_query("SELECT name FROM {workout_gym_affiliation} where uid=:uid", array(':uid' => $ts->creator_id))->fetchField();
		}
		$top_ten[$ts->wid] = convert_timestamp($ts->date) . ', Created by ' . $creator;
	}
	foreach($top_conditioning as $ts){
		// check to see if this timestamp is already in the array
		// If it is, it also has a strength portion so don't add it again
		if(!isset($top_ten[$ts->wid])){
			if($ts->creator_id == $user->uid){
				$creator = "You";
			}
			else{
				$creator = db_query("SELECT name FROM {workout_gym_affiliation} where uid=:uid", array(':uid' => $ts->creator_id))->fetchField();
			}
			$top_ten[$ts->wid] = convert_timestamp($ts->date) . ', Created by ' . $creator;
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

	$mobile = detect_mobile();

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
							if($k == ($p + 1) && ($rep_scheme[$p] != $rep_scheme[$k]  || $percentage_scheme[$p] != $percentage_scheme[$k])){
								$movement_str .= $rep_scheme[$p] . " @ " . $percentage_scheme[$p] . "%, ";
								break;
							}
							// This means we found some that are the same
							//drupal_set_message('<pre>' . print_r("p: " . $p . "\nk: " . $k, TRUE) . '</pre>');
							if($k == sizeof($percentage_scheme)){	
								$num_sets = $k + 1 - $p; 
							}
							else{
								$num_sets = $k - $p;
							}

							$movement_str .= $num_sets . "x" . $rep_scheme[$p] . " @ " . $percentage_scheme[$p] . "%, ";

							if($k == sizeof($percentage_scheme)){
								$p = $k;
							}			
							else{
								$p = $k - 1;
							}
							break;
						}
					}
				}

				$movement_str .= pretty_print_movement($workout[0]->movement);
			}

			else{
				if(!empty($workout[0]->rep_scheme)){
					$movement_str .= $workout[0]->rep_scheme . ',';
				}
				$movement_pretty = pretty_print_movement($workout[0]->movement);
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

			if($mobile == TRUE){
				$pretty_print = pretty_print_movement($workout[0]->movement);
				if(strlen($pretty_print) > 40){
					$pretty_print = "Strength Portion";
				}
				$form['strength'][$workout[0]->strength_id] = array(
					'#type' => 'fieldset',
					'#title' => t('@header', array('@header' => $pretty_print)),
					'#collapsible' => TRUE,
					'#collapsed' => TRUE,
				);
			}
			else {
				$form['strength'][$workout[0]->strength_id] = array(
					'#type' => 'fieldset',
					'#title' => t('@header', array('@header' => $movement_str)),
					'#collapsible' => TRUE,
					'#collapsed' => TRUE,
				);
			}
			$workout_print = $movement_str;
			// print the workout line
			$form['strength'][$workout[0]->strength_id]['header'] = array(
				'#markup' => '<p>' . $movement_str . '</p>',
				//'#markup' => '<p>' . build_strength_text($wid) . '</p>',
			);	

			if(isset($strength_notes[$workout[0]->strength_id]->notes)){
				// explode on line breaks and then format them as their own paragraph

				$paragraphs = explode("\n", $strength_notes[$workout[0]->strength_id]->notes);
				for($sn = 0; $sn < sizeof($paragraphs); $sn++){
					$form['strength'][$workout[0]->strength_id]['strength_notes'][$sn] = array(
						'#markup' => '<p>' . $paragraphs[$sn] . '</p>',
					);	
					$workout_print .= "\n\n" . $paragraphs[$sn];
				}
			}

			$form_state['strength_print'][$workout[0]->strength_id] = $workout_print;
	
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
				// save this to be used when submitting
				$form_state['reps_array'] = $reps;

				if(sizeof($percentage_scheme) > 1){
					$lift = $workout[0]->movement;
					if($complex_strength == FALSE){
						// if there is only one movement, then populate the default with the % from the PR
						global $user;
						$pr = db_query('SELECT pr from {workout_pr} where athlete_uid=:auid and movement=:movement', array(':auid' => $user->uid, ':movement' => $lift))->fetchField();
						$prescribed = round(($pr * ($percentage_scheme[$k-1] / 100)) / 5) * 5;
						if(sizeof($reps_array) == 1){
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
						if(sizeof($reps_array) == 1){
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
						'#type' => 'checkboxes',
						'#title' => t('Select ALL missed reps:'),
						'#options' => $reps_array,
						//'#default_value' => key($reps_array),
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
					for($nr = 1; $nr <= $reps_array[sizeof($reps_array)]; $nr++){
						// use this info to build form elements
						$form['strength'][$workout[0]->strength_id][$k]['fail_lift'][$nr] = array(
							'#type' => 'radios',
							'#title' => t('Failed on Rep @rep: movement', array('@rep' => $nr)),
							'#options' => $lift_keys,
							'#default_value' => key($lift_keys),
							'#states' => array(
								'visible' => array(
									':input[name="strength[' . $workout[0]->strength_id . '][' . $k . '][fail_rep][' . $nr . ']"]' => array('checked' => TRUE),
								),
							),
						);
					}
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
				$fieldset_title = $movement_str; 
				if(!empty($workout[0]->cutoff)){
					$movement_str = $movement_str . ", " . $workout[0]->cutoff . " minute cutoff:<br>";	
				}
				else{
					$movement_str = $movement_str . ":<br>";
				}
			}
			elseif($workout[0]->style == 'intervals' && $workout[0]->rounds > 1){
				if(!empty($workout[0]->duration)){
					(strpos($workout[0]->duration, ':') !== FALSE) ? $clock = $workout[0]->duration : $clock = $workout[0]->duration . ":00";
					$movement_str = $workout[0]->rounds . " Rounds on a " . $clock . " Minute Clock:<br>";
				}
				else {
					$movement_str = $workout[0]->rounds . " Rounds of:<br>";
				}
				if($workout[0]->rounds == 1){
					$fieldset_str = '1 Round: Intervals';
				}
				else{
					$fieldset_title = $workout[0]->rounds . ' Rounds: Intervals';
				}
			}
			elseif($workout[0]->style == 'rest'){
				$fieldset_title = "Rest";
				$movement_str = $workout[0]->duration . " Minutes Rest";
			}
			
			// replace pipes with html new lines
			$movement_str .= $workout[0]->workout . "<br>"; //str_replace("\n", "<br>", $workout[0]->workout);


			//drupal_set_message('<pre>' . print_r($movement_str, TRUE) . '</pre>');
			// Now that the string is done, build the fieldset to display it.
			if($workout[0]->style == 'rest'){
				$form['conditioning'][$workout[0]->conditioning_id] = array(
						'#type' => 'fieldset',
						'#title' => t('@header', array('@header' => $fieldset_title)),
						'#collapsible' => FALSE,
				);
			}
			else{
				$form['conditioning'][$workout[0]->conditioning_id] = array(
						'#type' => 'fieldset',
						'#title' => t('@header', array('@header' => $fieldset_title)),
						'#collapsible' => TRUE,
						'#collapsed' => TRUE,
				);
			}

				// print the workout line
				$form['conditioning'][$workout[0]->conditioning_id]['header'] = array(
					'#markup' => '<p>' . $movement_str . '</p>',
				);	

				$notes = '';
				if(isset($conditioning_notes[$workout[0]->conditioning_id]->notes)){
					$paragraphs = explode("\n", $conditioning_notes[$workout[0]->conditioning_id]->notes);
					for($cn = 0; $cn < sizeof($paragraphs); $cn++){
						$form['conditioning'][$workout[0]->conditioning_id]['conditioning_notes'][$cn] = array(
							'#markup' => '<p>' . $paragraphs[$cn] . '</p>',
						);
						$notes .= $paragraphs[$cn];
					}	
				}
			if(substr_count($movement_str, "Rest") != 1){	
				$form_state['conditioning_print'][$workout[0]->conditioning_id] = $movement_str . "\n" . $notes;
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
					if(empty($rounds) || !isset($rounds)){
						$rounds = 1;
					}
					$work = $workout[0]->workout;

					$intervals_full = explode("<br>", $work);
					$intervals = array();
					foreach($intervals_full as $invt){
						if(substr_count($invt, "rest") == 0){
							$intervals[] = $invt;
						}
					}
					$num_moves = sizeof($intervals);
					$form_state['interval_rounds'] = $rounds;
					$form_state['interval_num_moves'] = $num_moves;
					//drupal_set_message('<pre>' . print_r($num_intervals, TRUE) . '</pre>');
				
					for($interval = 1; $interval <= $rounds; $interval++){
						if(!empty($workout[0]->duration)){
							$form['conditioning'][$workout[0]->conditioning_id]['intervals'][$interval] = array(
								'#type' => 'fieldset',
								'#title' => t('Round #@int_num', array('@int_num' => $interval)),
								'#collapsible' => TRUE,
								'#collapsed' => TRUE,
							);
				
							$form['conditioning'][$workout[0]->conditioning_id]['intervals'][$interval]['split'] = array(
									'#type' => 'textfield',
									'#title' => t('Time'),
									'#size' => 5,
									'#maxlength' => 15,
								);
						}
						else {
							$form['conditioning'][$workout[0]->conditioning_id]['intervals'][$interval] = array(
								'#type' => 'fieldset',
								'#title' => t('Round #@int_num', array('@int_num' => $interval)),
								'#collapsible' => TRUE,
								'#collapsed' => TRUE,
							);
							for($move = 1; $move <= $num_moves; $move++){
								if(substr_count($intervals[$move - 1], "rest") == 0){
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

function pretty_print_movement($movement){
	$movement_pretty = implode(' ', array_map('ucfirst', explode(' ', $movement)));
	$movement_pretty = implode('-', array_map('ucfirst', explode('-', $movement_pretty)));
	$movement_pretty = implode('[', array_map('ucfirst', explode('[', $movement_pretty)));
	$movement_pretty = implode(']', array_map('ucfirst', explode(']', $movement_pretty)));
	$movement_pretty = implode('+', array_map('ucfirst', explode('+', $movement_pretty)));
	return $movement_pretty;
}

function detect_mobile(){
	$useragent=$_SERVER['HTTP_USER_AGENT'];

	// do the mobile detection
	if (
			preg_match('/android.+mobile|avantgo|bada\/|blackberry|blazer|compal|elaine|fennec|hiptop|iemobile|ip(hone|od)|iris|kindle|lge |maemo|meego.+mobile|midp|mmp|netfront|opera m(ob|in)i|palm( os)?|phone|p(ixi|re)\/|plucker|pocket|psp|series(4|6)0|symbian|treo|up\.(browser|link)|vodafone|wap|windows (ce|phone)|xda|xiino/i',$useragent)
			||
			preg_match('/1207|6310|6590|3gso|4thp|50[1-6]i|770s|802s|a wa|abac|ac(er|oo|s\-)|ai(ko|rn)|al(av|ca|co)|amoi|an(ex|ny|yw)|aptu|ar(ch|go)|as(te|us)|attw|au(di|\-m|r |s )|avan|be(ck|ll|nq)|bi(lb|rd)|bl(ac|az)|br(e|v)w|bumb|bw\-(n|u)|c55\/|capi|ccwa|cdm\-|cell|chtm|cldc|cmd\-|co(mp|nd)|craw|da(it|ll|ng)|dbte|dc\-s|devi|dica|dmob|do(c|p)o|ds(12|\-d)|el(49|ai)|em(l2|ul)|er(ic|k0)|esl8|ez([4-7]0|os|wa|ze)|fetc|fly(\-|_)|g1 u|g560|gene|gf\-5|g\-mo|go(\.w|od)|gr(ad|un)|haie|hcit|hd\-(m|p|t)|hei\-|hi(pt|ta)|hp( i|ip)|hs\-c|ht(c(\-| |_|a|g|p|s|t)|tp)|hu(aw|tc)|i\-(20|go|ma)|i230|iac( |\-|\/)|ibro|idea|ig01|ikom|im1k|inno|ipaq|iris|ja(t|v)a|jbro|jemu|jigs|kddi|keji|kgt( |\/)|klon|kpt |kwc\-|kyo(c|k)|le(no|xi)|lg( g|\/(k|l|u)|50|54|\-[a-w])|libw|lynx|m1\-w|m3ga|m50\/|ma(te|ui|xo)|mc(01|21|ca)|m\-cr|me(di|rc|ri)|mi(o8|oa|ts)|mmef|mo(01|02|bi|de|do|t(\-| |o|v)|zz)|mt(50|p1|v )|mwbp|mywa|n10[0-2]|n20[2-3]|n30(0|2)|n50(0|2|5)|n7(0(0|1)|10)|ne((c|m)\-|on|tf|wf|wg|wt)|nok(6|i)|nzph|o2im|op(ti|wv)|oran|owg1|p800|pan(a|d|t)|pdxg|pg(13|\-([1-8]|c))|phil|pire|pl(ay|uc)|pn\-2|po(ck|rt|se)|prox|psio|pt\-g|qa\-a|qc(07|12|21|32|60|\-[2-7]|i\-)|qtek|r380|r600|raks|rim9|ro(ve|zo)|s55\/|sa(ge|ma|mm|ms|ny|va)|sc(01|h\-|oo|p\-)|sdk\/|se(c(\-|0|1)|47|mc|nd|ri)|sgh\-|shar|sie(\-|m)|sk\-0|sl(45|id)|sm(al|ar|b3|it|t5)|so(ft|ny)|sp(01|h\-|v\-|v )|sy(01|mb)|t2(18|50)|t6(00|10|18)|ta(gt|lk)|tcl\-|tdg\-|tel(i|m)|tim\-|t\-mo|to(pl|sh)|ts(70|m\-|m3|m5)|tx\-9|up(\.b|g1|si)|utst|v400|v750|veri|vi(rg|te)|vk(40|5[0-3]|\-v)|vm40|voda|vulc|vx(52|53|60|61|70|80|81|83|85|98)|w3c(\-| )|webc|whit|wi(g |nc|nw)|wmlb|wonu|x700|yas\-|your|zeto|zte\-/i',substr($useragent,0,4))
	)
		{
			return TRUE;
		}
	return FALSE;
}

function workout_tracker_save_submit($form, &$form_state){
	if(isset($form_state['strength_exists'])){
		_save_strength_results($form_state);
	}
	if(isset($form_state['conditioning_exists']) && $form_state['conditioning_exists'] == TRUE){
		_save_conditioning_results($form_state);
	}
	send_results_email($form_state);
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
	$reps_array = $form_state['reps_array'];
	$strength_summary = array();

	for($i = 1; isset($form_state['input']['strength'][$i]); $i++){
		$workout = $form_state['input']['strength'][$i];
		$workout_completed = FALSE;
		$lifts = array_keys($all_lifts[$i]);
		$set_summary = '';
		for($s = 1; isset($workout[$s]); $s++){
			$set = $workout[$s];

			$record = new stdClass();
			$record->wid = $form_state['wid'];	
			$record->sid = $i;
			$record->athlete_uid = $user->uid;
			$record->num_performed = $num_performed;
			$record->set_num = $s;
			$set_summary = "Set #" . $s . ":";
			if(isset($set['work']) && !empty($set['work'])){
				$workout_completed = TRUE;
				$record->work = $set['work'];
				$set_summary .= " " . $set['work'] . "\n";
			}
//			drupal_set_message('<pre>' . print_r($set, TRUE) . '</pre>');
			if(isset($set['fail'])){
				$num_failed = 0;
				$failed_reps = '';
				$failed_movements = '';
				for($f = 1; $f <= $reps_array[$s - 1]; $f++){
					if(!empty($set['fail_rep'][$f])){
						($failed_reps == '') ? $failed_reps .= $set['fail_rep'][$f] : $failed_reps .= ':' . $set['fail_rep'][$f];
						$set_summary .= "     Failed on rep #" . $set['fail_rep'][$f];
						if(sizeof($lifts) == 1){ 
							($failed_movements == '') ? $failed_movements .= $lifts[0] : $failed_movements .= ':' . $lifts[0];
//							$set_summary .= $lifts[0] . "\n";
						}
						else {
							($failed_movements == '') ? $failed_movements .= $set['fail_lift'][$f] : $failed_movements .= ':' . $set['fail_lift'][$f];
							$set_summary .= ": " . $set['fail_lift'][$f] . "\n";
						}
					} 
				}
				if($failed_reps != ''){
					// TODO fill in lift_results excluding failed movements
					$failed = explode(":", $failed_reps);
					if(sizeof($failed) < $reps_array[$s - 1]){
						// this means at least one rep was completed, find it and set it if its bigger than what's already there
						foreach($lifts as $lift){
							if($set['work'] > $lift_results[$lift]){
								$lift_results[$lift] = $set['work'];
							}
						}
					}
					else {
						// check to see if any movements were successful
						for($j = 1; $j <= $reps_array[sizeof($reps_array) - 1]; $j++){
							$failed_moves = explode(":", $failed_movements);
							foreach($lifts as $lift){
								if($lift != $failed_moves[0] && $set['work'] > $lift_results[$lift]){
									$lift_results[$lift] = $set['work'];
								}
								else{
									break;
								}
							}
						}
					}
					//drupal_set_message('<pre>' . print_r("Failed Rep String " . $failed_reps, TRUE) . '</pre>');
					//drupal_set_message('<pre>' . print_r("Failed Movement String " . $failed_movements, TRUE) . '</pre>');
					$record->fail = $failed_reps;
					$record->fail_movement = $failed_movements;
				}
			}
			else{
				// didn't fail, so map weights to lifts
				foreach($lifts as $lift){
					$lift_results[$lift] = $set['work'];
				}
			}
			if($workout_completed == TRUE){
				$strength_summary[$i][$s] = $set_summary;	
				drupal_write_record($strength_table, $record);
			}
		}
	}
	foreach(array_keys($lift_results) as $l){
		//drupal_set_message('<pre>' . print_r($l, TRUE) . '</pre>');
		check_pr($l, $lift_results[$l]);
	}
	$form_state['strength_summary'] = $strength_summary;
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

	$conditioning_summary = '';
	$workout_summary = '';
	//drupal_set_message('<pre>' . print_r($form_state['input']['conditioning'], TRUE) . '</pre>');

	$cids = array_keys($form_state['input']['conditioning']);

	for($iter = 0; $iter < sizeof($cids); $iter++){
		$i = $cids[$iter];
		$workout = $form_state['input']['conditioning'][$i];
		//drupal_set_message('<pre>' . print_r($workout, TRUE) . '</pre>');
		$workout_completed = FALSE;
		$workout_summary = '';
		$record = new stdClass();
		$record->wid = $form_state['wid'];
		$record->cid = $i;
		$record->athlete_uid = $user->uid;
		$record->num_performed = $num_performed;	

		if(!empty($workout['amrap_rounds']) || !empty($workout['amrap_reps'])){
			$workout_completed = TRUE;
			$workout_summary .= "Finished: ";
			if(isset($workout['amrap_rounds'])){
				$record->amrap_rounds = $workout['amrap_rounds'];
				$workout_summary .= $record->amrap_rounds . " rounds";
			}
			else{
				$record->amrap_round = 0;
			}

			if(isset($workout['amrap_reps'])){
				$record->amrap_reps = $workout['amrap_reps'];
				$workout_summary .= " " . $record->amrap_reps . " reps";
			}
			else {
				$record->amrap_reps = 0;
			}
		}
		
		if(!empty($workout['for_time'])){
			$workout_completed = TRUE;
			$record->time = $workout['for_time'];
			(strpos(":", $record->time) !== FALSE) ? $workout_summary .= "Finished in " . $record->time . " minutes" : $workout_summary .= "Finished in " . $record->time . ":00 minutes";
		}
		
		if(isset($workout['intervals'])){
			$results = '';
			for($interval = 1; $interval <= $form_state['interval_rounds']; $interval++){
				if(!empty($workout['intervals'][$interval]['split'])){
					$workout_completed = TRUE;
					$results .= $workout['intervals'][$interval]['split'];
					if(strpos(":", $workout['intervals'][$interval]['split']) !== FALSE){
						$workout_summary .= "Round #" . $interval . ": " . $workout['intervals'][$interval]['split'] . " minutes\n";
					}
					else {
						$workout_summary .= "Round #" . $interval . ": " . $workout['intervals'][$interval]['split'] . ":00 minutes\n";
					}
				}
				else {
					$workout_summary .= "Round #" . $interval . ":";
					for($move = 1; $move <= $form_state['interval_num_moves']; $move++){		
						if(!empty($workout['intervals'][$interval][$move])){
							$workout_completed = TRUE;
							($move > 1) ? $results .= ',' . $workout['intervals'][$interval][$move] : $results .= $workout['intervals'][$interval][$move];
							$workout_summary .= " " . $workout['intervals'][$interval][$move];
						}
					}
					$workout_summary .= "\n";
				}
				if($interval < $form_state['interval_rounds']){
					$results .= '-';
				}
			}
			$record->interval_reps = $results;
	//		drupal_set_message('<pre>' . print_r($record->interval_reps, TRUE) . '</pre>');
		}

		if($workout_completed == TRUE){
		//	drupal_set_message('<pre>' . print_r($workout_summary, TRUE) . '</pre>');
			drupal_write_record($conditioning_table, $record);
			$form_state['conditioning_summary'][$i] = $workout_summary;
		}
	}
	//drupal_set_message('<pre>' . print_r($form_state['conditioning_summary'], TRUE) . '</pre>');
}

function check_pr($lift, $weight){
	global $user;
//	drupal_set_message('<pre>' . print_r($lift . ':' . $weight, TRUE) . '</pre>');
	$pr_table = 'workout_pr';
	$movement = strtolower($lift);

	$cur_pr = db_query('SELECT pr FROM {workout_pr} where athlete_uid=:auid AND movement=:movement', array(':auid' => $user->uid, ':movement' => $movement))->fetchField();

	$record = new stdClass();
	$record->athlete_uid = $user->uid;
	$record->date = time(); 

	if(!empty($cur_pr)){
		if($weight > $cur_pr){
			$num_updated = db_update($pr_table)
				->fields(array(
					'pr' => $weight,
				))
				->condition('athlete_uid', $user->uid, '=')
				->condition('movement', $movement, '=')
				->execute();
			//$result = db_query('UPDATE {workout_pr} SET pr=:pr WHERE athlete_uid=:auid and movement=:movement', array(':pr' => $weight, ':auid' => $user->uid, ':movement' => $movement));
			if($num_updated != 1){
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

function workout_tracker_mail($key, &$message, $params){
	global $user;

	$options = array(
		'langcode' => $message['language']->language,
	);

	switch($key){
		case 'workout_tracker_results':
			$message['subject'] = t('Workout Results for @date', array('@date' => convert_timestamp(time())), $options);
			$message['body'][] = t('Great work today @name!', array('@name' => $user->name));
			$message['body'][] = t('Here are you results from today\'s workout:');
			for($i = 1; isset($params['strength_print'][$i]); $i++){
				$workout = '';
				for($s = 1; isset($params['strength_summary'][$i][$s]); $s++){ 
					$workout .= t($params['strength_summary'][$i][$s]);
				}
				if($workout != ''){
					$message['body'][] = t("-----------------------------------\n");
					$message['body'][] = t($params['strength_print'][$i]);
					$message['body'][] = t("\nResults:");
					$message['body'][] = $workout;
					$message['body'][] = t("-----------------------------------\n");
				}
			}

			$cids = array_keys($params['conditioning_print']);
			for($iter = 0; $iter < sizeof($cids); $iter++){
				$i = $cids[$iter];
				if(!empty($params['conditioning_summary'][$i])){
					$message['body'][] = $params['conditioning_print'][$i];			
					$message['body'][] = t("\n\nResults:");
					$message['body'][] = $params['conditioning_summary'][$i];
				}
			}

			break;
	}
}

function send_results_email(&$form_state){
	global $user;	

	$module = 'workout_tracker';
	$key = 'workout_tracker_results';

	$to = $user->mail;
	$from = 'WODology@wodology.com';
	$params = $form_state;
	$language = language_default();
	$send = TRUE;
	$result = drupal_mail($module, $key, $to, $language, $params, $from, $send);
	if($result['result'] == TRUE){
//		drupal_set_message('Message was sent');
	}
	else {
//		drupal_set_message('Message failed', 'error');
	}
}
