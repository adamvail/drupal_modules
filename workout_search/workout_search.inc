<?php

function workout_search($form, &$form_state){
	global $user;

	if(!empty($form_state['page_num']) && $form_state['page_num'] == 2){
		return workout_search_find_workouts($form, $form_state);
	}

	$form_state['page_num'] = 1;

	$form['search'] = array(
		'#type' => 'textfield',
		'#title' => t('Movement'),
		'#size' => 15,
		'#maxlength' => 30, 
	);	
	
	$options = array('strength' => t('Strength'), 'conditioning' => t('Conditioning'), 'both' => t('Both'));
	$form['search_type'] = array(
		'#type' => 'radios',
		'#title' => t('Search strength, conditioning, or both'),
		'#options' => $options,
		'#default_value' => 'both',
	);

	$form['next'] = array(
		'#type' => 'submit',
		'#value' => t('Search'),
		'#submit' => array('workout_search_next_submit'),
	);

	return $form;

}

function workout_search_next_submit($form, &$form_state){
//	drupal_set_message('<pre>' . print_r($form_state, TRUE) . '</pre>');

	$form_state['search_term'] = $form_state['input']['search'];
	$form_state['search_type'] = $form_state['input']['search_type'];
	$form_state['page_num'] = 2;
	$form_state['rebuild'] = TRUE;
}

function workout_search_find_workouts($form, &$form_state){
	global $user;
	$search_term = $form_state['search_term'];
	$search_type = $form_state['search_type'];
	$workouts_strength = array();
	$workouts_conditioning = array();

	$gym = db_query('SELECT gym FROM {workout_gym_affiliation} where uid=:uid', array(':uid' => $user->uid))->fetchField();

	if(isset($gym) && !empty($gym)){
		// don't grab everyone from the gym's workouts, only grab works from this gym where the coach created it.
		$coaches = db_query('SELECT uid from {workout_gym_affiliation} where gym=:gym and role=:role', array(':gym' => $gym, ':role' => 'coach'));
		$creator_ids = array();
		foreach($coaches as $c){
			$creator_ids[] = $c->uid;
		}
		$creator_ids[] = $user->uid;
		if($search_type == 'both'){
			$workouts_strength = db_query("SELECT wid, strength_id FROM {workout_builder_strength} WHERE creator_id IN (:cid) AND movement LIKE :mvmt", array(':cid' => $creator_ids, ':mvmt' => "%" . db_like($search_term) . "%"))->fetchAll();
			$workouts_conditioning = db_query("SELECT wid, conditioning_id FROM {workout_builder_conditioning} WHERE creator_id IN (:cid) AND workout LIKE :mvmt", array(':cid' => $creator_ids, ':mvmt' => "%" . db_like($search_term) . "%"))->fetchAll();
		}
		elseif($search_type == 'strength'){
			$workouts_strength = db_query("SELECT wid, strength_id FROM {workout_builder_strength} WHERE creator_id IN (:cid) AND movement LIKE :mvmt", array(':cid' => $creator_ids, ':mvmt' => "%" . db_like($search_term) . "%"))->fetchAll();
		}
		elseif($search_type == 'conditioning'){
			$workouts_conditioning = db_query("SELECT wid, conditioning_id FROM {workout_builder_conditioning} WHERE creator_id IN (:cid) AND workout LIKE :mvmt", array(':cid' => $creator_ids, ':mvmt' => "%" . db_like($search_term) . "%"))->fetchAll();
		}
	}
	else {
		if($search_type == 'both'){
			$workouts_strength = db_query("SELECT wid, strength_id FROM {workout_builder_strength} WHERE creator_id=:cid AND movement LIKE :mvmt", array(':cid' => $user->uid, ':mvmt' => '%' . db_like($search_term) . '%'))->fetchAll();
			$workouts_conditioning = db_query("SELECT wid, conditioning_id FROM {workout_builder_conditioning} WHERE creator_id=:cid AND workout LIKE :mvmt", array(':cid' => $user->uid, ':mvmt' => '%' . db_like($search_term) . '%'))->fetchAll();
		}
		elseif($search_type == 'strength'){
			$workouts_strength = db_query("SELECT wid, strength_id FROM {workout_builder_strength} WHERE creator_id=:cid AND movement LIKE :mvmt", array(':cid' => $user->uid, ':mvmt' => '%' . db_like($search_term) . '%'))->fetchAll();
		}
		elseif($search_type == 'conditioning'){
			$workouts_conditioning = db_query("SELECT wid, conditioning_id FROM {workout_builder_conditioning} WHERE creator_id=:cid AND workout LIKE :mvmt", array(':cid' => $user->uid, ':mvmt' => '%' . db_like($search_term) . '%'))->fetchAll();
		}
	}

	krsort($workouts_strength);
	krsort($workouts_conditioning);

	// figure out which portions need to be printed

	foreach($workouts_strength as $workout){
		//drupal_set_message('<pre>' . print_r($workout, TRUE) . '</pre>');
		workout_search_print_results($form, $form_state, $workout->wid, $workout->strength_id, 'strength');
	}

	foreach($workouts_conditioning as $workout){
		workout_search_print_results($form, $form_state, $workout->wid, $workout->conditioning_id, 'conditioning');
	}
	return $form;
}

function workout_search_print_results(&$form, &$form_state, $wid, $sub_id, $which){
	global $user;
	$form_state['wid'] = $wid;
	$count = 1;

	if($which != 'strength' && $which != 'conditioning'){
		return;
	}

	$mobile = detect_mobile();

	($which == 'strength') ? $max_performed = db_query("SELECT MAX(num_performed) FROM {workout_tracker_strength} WHERE wid=:wid AND athlete_uid=:aid", array(':wid' => $wid, ':aid' => $user->uid))->fetchField() : 
			$max_performed = db_query("SELECT MAX(num_performed) FROM {workout_tracker_conditioning} WHERE wid=:wid AND athlete_uid=:aid", array(':wid' => $wid, ':aid' => $user->uid))->fetchField();

	(isset($form['workout'])) ? $initial_size = sizeof($form['workout']) : $initial_size = 0;
		//drupal_set_message('<pre>' . print_r("max performed: " . $max_performed, TRUE) . '</pre>');
	for($i = $initial_size + 1; $i <= $max_performed + $initial_size; $i++){
		//drupal_set_message('<pre>' . print_r($i, TRUE) . '</pre>');

		$strength_str = '';
		$conditioning_str = '';

		if($which == 'strength'){
			$strength_str = create_strength_string($wid, $sub_id, $count);
		}
		elseif($which == 'conditioning'){
			$conditioning_str = create_conditioning_string($wid, $sub_id, $count);
		}

		$date = db_query("SELECT DISTINCT(date) FROM {workout_tracker_strength} where wid=:wid AND athlete_uid=:aid AND num_performed=:np", array(':wid' => $wid, ':aid' => $user->uid, ':np' => $count))->fetchField();

		if(empty($date)){
			$date = db_query("SELECT DISTINCT(date) FROM {workout_tracker_conditioning} where wid=:wid AND athlete_uid=:aid AND num_performed=:np", array(':wid' => $wid, ':aid' => $user->uid, ':np' => $count))->fetchField();

		}

		if(!empty($strength_str) || !empty($conditioning_str)){
			$form['workout'][$i] = array(
				'#type' => 'fieldset',
				'#title' => t("Performed on @date", array('@date' => convert_timestamp($date))),
				'#collapsible' => TRUE,
				'#collapsed' => TRUE,
			);
		}

		if(!empty($strength_str)){
		/*	$form['workout'][$i]['strength'] = array(
				'#type' => 'fieldset',
				'#title' => t('Strength'),
			);
*/
			$form['workout'][$i]['strength'] = array(
				'#markup' => $strength_str, 
			);
		}

		if(!empty($conditioning_str)){
/*			$form['workout'][$i]['conditioning'] = array(
				'#type' => 'fieldset',
				'#title' => t('Conditioning'),
			);
*/
			$form['workout'][$i]['conditioning'] = array(
				'#markup' => $conditioning_str, 
			);
		}
		$count ++;
	}

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

function create_strength_string($wid, $strength_id, $num_performed){
	global $user;

	$headers = assemble_strength_headers($wid, $strength_id);
	$sids = array_keys($headers);
	for($i = 0; $i < sizeof($sids); $i++){
		$workout_result = '';
		$sid = $sids[$i];
		$results_headers = create_results_array($wid, $sid);
		$strength_results = fillin_strength_results($results_headers, $wid, $sid, $num_performed);
		if($strength_results == ''){
			return '';
		}
		$workout_result .= $headers[$sid] . "<br>";
		foreach($strength_results as $sr){
			$workout_result .= "<br>" . $sr . "<br>";
		}
		//drupal_set_message('<pre>' . print_r($workout_result, TRUE) . '</pre>');
		return $workout_result;
	}
}

function fillin_strength_results($headers, $wid, $sid, $num_performed){
	global $user;
	

	$strength_results = db_query('SELECT * from {workout_tracker_strength} where wid=:wid AND athlete_uid=:aid AND sid=:sid AND num_performed=:np', array(':wid' => $wid, ':aid' => $user->uid, ':sid' => $sid, ':np' => $num_performed))->fetchAll();

	if(empty($strength_results)){
		return '';
	}
	
	// group the results by num_performed
	$strength = array();
	foreach($strength_results as $sr){
		$strength[$sr->num_performed][] = $sr;
	}

	$results = array();
	$keys = array_keys($strength);
	foreach($keys as $key){
		$results[$key] = ''; //"Performed on " . convert_timestamp($strength[$key][0]->date);
		for($i = 0; $i < sizeof($strength[$key]); $i++){
			($i == 0) ? $results[$key] .= $headers[$i] . $strength[$key][$i]->work : $results[$key] .= '<br>' . $headers[$i] . $strength[$key][$i]->work; 
			if(!empty($strength[$key][$i]->fail)){
				// There was a fail on this set
				$failed_reps = explode(":", $strength[$key][$i]->fail);
				$failed_movements = explode(":", $strength[$key][$i]->fail_movement);
				for($k = 0; $k < sizeof($failed_reps); $k++){
					$results[$key] .= "<br>     " . "Failed on rep " . $failed_reps[$k];
					if(isset($failed_movements[$k])){
						$results[$key] .= ": Movement: " . pretty_print_movement($failed_movements[$k]);
					}
				}
			}
		}
	}

//	drupal_set_message('<pre>' . print_r($results, TRUE) . '</pre>');
	return $results;
}

function create_results_array($wid, $sid){
	$strength = db_query('SELECT * FROM {workout_builder_strength} where wid=:wid AND strength_id=:sid', array(':wid' => $wid, 'sid' => $sid));
	$results_headers = array();
	if(!empty($strength)){
		$strength_data = array();	
	
		// put the rows from the db into an array for looping by strength_id
		foreach($strength as $movement){
			// group by the strength id for form building
			$strength_data[$movement->strength_id][] = $movement;
		}

		$all_lifts = array();
		foreach($strength_data as $workout){
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

			$percentage_scheme = explode("-", $workout[0]->wgt_percentage);
			// Standard form items for all movements, regardless of complexity
			for($k = 1; $k <= $sets; $k++){
				$movement_str = '';
				$reps_array = array();
				for($j = 1; $j <= $reps[$k-1]; $j++){
					$reps_array[$j] = $j;
				}

				if(sizeof($percentage_scheme) > 1){
					$lift = $workout[0]->movement;
					if(sizeof($reps_array) == 1){
						$movement_str .= "1 Rep: " . $lift . " @ " . $percentage_scheme[$k-1] . "%: " ; 
					}
					else {
						$movement_str .= sizeof($reps_array) . " Reps: " . $lift . " @ " . $percentage_scheme[$k-1] . "%: " ; 
					}
				}
				else {
					$set_header = pretty_print_movement($workout[0]->movement);	
					if(sizeof($reps_array) == 0){
						$movement_str .= $set_header . ": "; 
					}
					elseif(sizeof($reps_array) == 1) {
						$movement_str .= "1 Rep: " . $set_header .": "; 
					}
					else {
						$movement_str .= sizeof($reps_array) . " Reps: " . $set_header . ": ";
					}
				}
				$results_headers/*[$workout[0]->strength_id]*/[] = $movement_str;				
	//			drupal_set_message('<pre>' . print_r($movement_str, TRUE) . '</pre>');
			}
		}
	}
//	drupal_set_message('<pre>' . print_r($results_headers, TRUE) . '</pre>');
	return $results_headers;
}

function assemble_strength_headers($wid, $sid){
	$strength = db_query('SELECT * FROM {workout_builder_strength} where wid=:wid AND strength_id=:sid', array(':wid' => $wid, ':sid' => $sid));
	$strength_notes_results = db_query('SELECT * FROM {workout_builder_strength_notes} where wid=:wid AND strength_id=:sid', array(':wid' => $wid, ':sid' => $sid));
	$headers = array();
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

		$all_lifts = array();
		$set_header = "";	
		foreach($strength_data as $workout){
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
							if($k == sizeof($percentage_scheme) && $percentage_scheme[$p] == $percentage_scheme[$k] && $rep_scheme[$p] == $rep_scheme[$k]){	
								$num_sets = $k + 1 - $p; 
							}
							else{
								$num_sets = $k - $p;
							}

							$movement_str .= $num_sets . "x" . $rep_scheme[$p] . " @ " . $percentage_scheme[$p] . "%, ";

							if($k == sizeof($percentage_scheme) && $percentage_scheme[$p] == $percentage_scheme[$k] && $rep_scheme[$p] == $rep_scheme[$k]){
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
			$headers[$workout[0]->strength_id] = $movement_str;
		}
	}
	return $headers;
}

function create_conditioning_string($wid, $num_performed){

	$headers = assemble_conditioning_headers($wid);
	$results = fill_in_conditioning_results($wid, $num_performed);

	if(empty($results)){
		return '';
	}

	$cids = array_keys($headers);
	$workout_str = "";
	for($i = 0; $i < sizeof($cids); $i++){
		$cid = $cids[$i];
		$workout_str .= $headers[$cid] . "<br>";
		if(substr_count($headers[$cid], "Rest") == 0){
			$workout_str .= "<br>" .  $results[$cid] . "<br>";
		}
		else{
			$workout_str .= "<br>";
		}
	}

//	drupal_set_message('<pre>' . print_r($workout_str, TRUE) . '</pre>');
	return $workout_str;
}

function create_interval_strings($wid, $cid){
	$conditioning = db_query('SELECT * FROM {workout_builder_conditioning} where wid=:wid AND conditioning_id=:cid', array(':wid' => $wid, ':cid' => $cid));
	$workout = $conditioning->fetchAll();
	$results = array();
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
		//drupal_set_message('<pre>' . print_r($num_intervals, TRUE) . '</pre>');
	
		for($interval = 1; $interval <= $rounds; $interval++){
			$movement_str = '';
			for($move = 1; $move <= $num_moves; $move++){
				if(substr_count($intervals[$move - 1], "rest") == 0){
					$movement_str .= "     " . trim(str_replace("\n", "<br>", $intervals[$move - 1])) . ":<br>";
				}				
			}
			$results[] = $movement_str;
		}
	}
//	drupal_set_message('<pre>' . print_r($results, TRUE) . '</pre>');
	return $results;
}

function fill_in_conditioning_results($wid, $num_performed){
	global $user;

	$conditioning_results = db_query("SELECT * FROM {workout_tracker_conditioning} where wid=:wid AND athlete_uid=:aid AND num_performed=:np", array(':wid' => $wid, ':aid' => $user->uid, ':np' => $num_performed))->fetchAll();

	$conditioning = array();
/*
	foreach($conditioning_results as $cr){
		$conditioning[$cr->num_performed][] = $cr;
	}
*/
	foreach($conditioning_results as $cr){
		$conditioning[$cr->cid][] = $cr;
	}

	$results = array();
	$keys = array_keys($conditioning);
	//drupal_set_message('<pre>' . print_r($keys, TRUE) . '</pre>');
	foreach($keys as $key){
		$results[$key] = ''; //"Performed on " . convert_timestamp($conditioning[$key][0]->date) . "<br>";
		for($i = 0; $i < sizeof($conditioning[$key]); $i++){
			$workout = $conditioning[$key][$i];
			// check to see what kind of conditioning workout it is
			if(isset($workout->time) && !empty($workout->time)){
				//drupal_set_message('<pre>' . print_r("Rounds for time:\n" . $workout, TRUE) . '</pre>');
				// rounds for time
				if(strpos($workout->time, ":") !== FALSE || strpos($workout->time, "+") !== FALSE){
					$results[$key] .= "     Time: " . $workout->time . " minutes<br>";
				}
				else {
					$results[$key] .= "     Time: " . $workout->time . ":00 minutes\n";
				}
/*				if(strpos($workout->time, ":") !== FALSE){
					$results[$key] .= "     Time: " . $workout->time . " minutes<br>";
				}
				else {
					$results[$key] .= "     Time: " . $workout->time . ":00 minutes\n";
				}*/
			}
			elseif(!empty($workout->amrap_rounds) || !empty($workout->amrap_reps)){
				// amrap
				if(!empty($workout->amrap_rounds)){
					$results[$key] .= "     " . $workout->amrap_rounds . " Rounds, ";
					if(!empty($workout->amrap_reps)){
						$results[$key] .= $workout->amrap_reps . " Reps";
					}
				}
				else{
					$results[$key] .= "     " . $workout->amrap_reps . " Reps" ;
				}
			}
			elseif(!empty($workout->interval_reps)){
				if(strpos($workout->interval_reps, ":") !== FALSE){
					$intervals = explode("-", $workout->interval_reps);
					for($k = 0; $k < sizeof($intervals); $k++){
						if(strpos($intervals[$k], ":") !== FALSE){
							$results[$key] .= "Round #" . ($k + 1) . ': ' . $intervals[$k] . " minutes<br>";
						}
						else {
							$results[$key] .= "Round #" . ($k + 1) . ': ' . $intervals[$k] . ":00 minutes<br>";
						}
					}
				}
				else{
					$moves = create_interval_strings($wid, $key);	
					//drupal_set_message('<pre>' . print_r($moves, TRUE) . '</pre>');
					$intervals = explode("-", $workout->interval_reps); 
					for($k = 0; $k < sizeof($intervals); $k++){
						$values = explode(",", $intervals[$k]);
						$interval_moves = explode("\n", $moves[$k]);
						//drupal_set_message('<pre>' . print_r($values, TRUE) . '</pre>');
						$results[$key] .= "Round #" . ($k + 1) . ":\n";
						for($j = 0; $j < sizeof($values); $j++){
							$results[$key] .= $interval_moves[$j] . $values[$j] . "<br>";
						}
					}

				}
//				drupal_set_message('<pre>' . print_r($moves, TRUE) . '</pre>');
			}
		}
	}
	
	//drupal_set_message('<pre>' . print_r($results, TRUE) . '</pre>');
	return $results;

}

function assemble_conditioning_headers($wid){
	$conditioning = db_query('SELECT * FROM {workout_builder_conditioning} where wid=:wid', array(':wid' => $wid));
	$conditioning_notes_results = db_query('SELECT * FROM {workout_builder_conditioning_notes} where wid=:wid', array(':wid' => $wid));
	$results = array();

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

			//$movement_str = "Performed on " . convert_timestamp($workout[0]->date) . "\n";
			$movement_str = '';

			if($workout[0]->style == 'amrap'){
				$movement_str .= $workout[0]->duration . " minute AMRAP:<br>";
			}
			elseif($workout[0]->style == 'for_time'){
				$movement_str .= $workout[0]->rounds . " Rounds For Time";
				if(!empty($workout[0]->cutoff)){
					$movement_str .= ", " . $workout[0]->cutoff . " minute cutoff:<br>";	
				}
				else{
					$movement_str .= ":<br>";
				}
			}
			elseif($workout[0]->style == 'intervals' && $workout[0]->rounds > 1){
				if(!empty($workout[0]->duration)){
					(strpos($workout[0]->duration, ':') !== FALSE) ? $clock = $workout[0]->duration : $clock = $workout[0]->duration . ":00";
					$movement_str .= $workout[0]->rounds . " Rounds on a " . $clock . " Minute Clock:<br>";
				}
				else {
					$movement_str .= $workout[0]->rounds . " Rounds of:<br>";
				}
			}
			elseif($workout[0]->style == 'rest'){
				$movement_str .= $workout[0]->duration . " Minutes Rest";
			}
			
			// replace pipes with html new lines
			$movement_str .= $workout[0]->workout;

			if(isset($conditioning_notes[$workout[0]->conditioning_id]->notes)){
				$movement_str .= "<br><br>" . $conditioning_notes[$workout[0]->conditioning_id]->notes;
			}
// insert header here
			$results[$workout[0]->conditioning_id] = $movement_str;
		}
	}
	//drupal_set_message('<pre>' . print_r($results, TRUE) . '</pre>');
	return $results;
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
