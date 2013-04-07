<?php

function workout_results($form, &$form_state){
	global $user;

	$form['#tree'] = TRUE;
	$cur_time = time();

	if(isset($_GET['wid'])){
		$form_state['values']['workout_choice'] = $_GET['wid'];
		return workout_results_print_results($form, $form_state);
	}

	if(!empty($form_state['page_num']) && $form_state['page_num'] == 2){
		return workout_results_print_results($form, $form_state);
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
		'#submit' => array('workout_results_next_submit'),
	);

	return $form;

}

function workout_results_next_submit($form, &$form_state){
//	drupal_set_message('<pre>' . print_r($form_state, TRUE) . '</pre>');

	$form_state['page_num'] = 2;
	$form_state['rebuild'] = TRUE;
}

function workout_results_print_results($form, &$form_state){
	// Build this form based off the workout they just chose
	$wid = $form_state['values']['workout_choice'];
	$form_state['wid'] = $wid;

	$mobile = detect_mobile();
	create_strength_string($wid);
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

function create_strength_string($wid){
	global $user;

	$headers = assemble_strength_headers($wid);
	$sids = array_keys($headers);
	for($i = 0; $i < sizeof($sids); $i++){
		$workout_result = '';
		$sid = $sids[$i];
		$results_headers = create_results_array($wid, $sid);
		$strength_results = fillin_strength_results($results_headers, $wid, $sid);
		$workout_result .= $headers[$sid] . "\n";
		foreach($strength_results as $sr){
			$workout_result .= "\n" . $sr . "\n";
		}
		drupal_set_message('<pre>' . print_r($workout_result, TRUE) . '</pre>');
	}
}

function fillin_strength_results($headers, $wid, $sid){
	global $user;
	
	// TODO Show all the performances
//	$latest_performance = db_query('SELECT MAX(num_performed) from {workout_tracker_strength} where wid=:wid AND athlete_uid=:aid AND sid=:sid', array(':wid' => $wid, ':aid' => $user->uid, ':sid' => $sid))->fetchField();

	$strength_results = db_query('SELECT * from {workout_tracker_strength} where wid=:wid AND athlete_uid=:aid AND sid=:sid', array(':wid' => $wid, ':aid' => $user->uid, ':sid' => $sid))->fetchAll();
	
	//drupal_set_message('<pre>' . print_r($strength_results, TRUE) . '</pre>');
	//drupal_set_message('<pre>' . print_r($headers, TRUE) . '</pre>');
/*
	if(sizeof($strength_results) != sizeof($headers)){
		drupal_set_message('<pre>' . print_r("Results are not the same size as the headers!", TRUE) . '</pre>');
		return;
	}	
*/

	// group the results by num_performed
	$strength = array();
	foreach($strength_results as $sr){
		$strength[$sr->num_performed][] = $sr;
	}

	$results = array();
	$keys = array_keys($strength);
	foreach($keys as $key){
		$results[$key] = "Performed on " . convert_timestamp($strength[$key][0]->date);
		for($i = 0; $i < sizeof($strength[$key]); $i++){
			$results[$key] .= "\n" . $headers[$i] . $strength[$key][$i]->work; 
			if(!empty($strength[$key][$i]->fail)){
				// There was a fail on this set
				$failed_reps = explode(":", $strength[$key][$i]->fail);
				$failed_movements = explode(":", $strength[$key][$i]->fail_movement);
				for($k = 0; $k < sizeof($failed_reps); $k++){
					$results[$key] .= "\n     " . "Failed on rep " . $failed_reps[$k];
					if(isset($failed_movements[$k])){
						$results[$key] .= ": Movement: " . pretty_print_movement($failed_movements[$k]);
					}
				}
			}
		}
	}

/*
	for($i = 0; $i < sizeof($strength_results); $i++){
		if(($i + 1) != $strength_results[$i]->set_num){
			drupal_set_message('<pre>' . print_r("Results are out of order!", TRUE) . '</pre>');
			return;
		}
		$headers[$i] .= $strength_results[$i]->work;
		if(!empty($strength_results[$i]->fail)){
			// There was a fail on this set
			$failed_reps = explode(":", $strength_results[$i]->fail);
			$failed_movements = explode(":", $strength_results[$i]->fail_movement);
			for($k = 0; $k < sizeof($failed_reps); $k++){
				$headers[$i] .= "\n     " . "Failed on rep " . $failed_reps[$k];
				if(isset($failed_movements[$k])){
					$headers[$i] .= ": Movement: " . pretty_print_movement($failed_movements[$k]);
				}
			}
		}
	}
*/
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

function assemble_strength_headers($wid){
	$strength = db_query('SELECT * FROM {workout_builder_strength} where wid=:wid', array(':wid' => $wid));
	$strength_notes_results = db_query('SELECT * FROM {workout_builder_strength_notes} where wid=:wid', array(':wid' => $wid));
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
			$headers[$workout[0]->strength_id] = $movement_str;
		}
	}
	return $headers;
}

function create_conditioning_string($wid){
	$conditioning = db_query('SELECT * FROM {workout_builder_conditioning} where wid=:wid', array(':wid' => $wid));
	$conditioning_notes_results = db_query('SELECT * FROM {workout_builder_conditioning_notes} where wid=:wid', array(':wid' => $wid));
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
