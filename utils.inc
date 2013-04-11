<?php

$drupal_root_dir = '/var/www';
chdir($drupal_root_dir);

//define('DRUPAL_ROOT', $drupal_root_dir);

require_once "/var/www/includes/bootstrap.inc";
drupal_bootstrap(DRUPAL_BOOTSTRAP_DATABASE);

function create_strength_string($wid){

	$strength = db_query('SELECT * FROM {workout_builder_strength} where wid=:wid', array(':wid' => $wid));
	$strength_notes_results = db_query('SELECT * FROM {workout_builder_strength_notes} where wid=:wid', array(':wid' => $wid));

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
			$movement_str = "";
			if(isset($workout[0]->wgt_style) && $workout[0]->wgt_style == 'percentage' && (sizeof(explode("-", $workout[0]->wgt_percentage)) > 1)){
				$percentage_scheme = explode("-", $workout[0]->wgt_percentage);
				$rep_scheme = explode("-", $workout[0]->rep_scheme);
			
				// try to coalesce sets together
				for($p = 0; $p < sizeof($percentage_scheme); $p++){
					//drupal_set_message('<pre>' . print_r($p, TRUE) . '</pre>');
					if($p == sizeof($percentage_scheme) - 1){
						// This hasn't been coalesced and won't be since it's the last one
						// so print it out
						$movement_str .= $rep_scheme[$p] . " @ " . $percentage_scheme[$p] . "%, ";
					}

					for($k = $p + 1; $k < sizeof($percentage_scheme); $k++){
						if((isset($percentage_scheme[$p],$percentage_scheme[$k],$rep_scheme[$p],$rep_scheme[$k]))){
							if( ($percentage_scheme[$p] != $percentage_scheme[$k] || 
									($percentage_scheme[$p] == $percentage_scheme[$k] && $k == sizeof($percentage_scheme) - 1) || 
									($rep_scheme[$p] != $rep_scheme[$k]))
							){
								if($k == $p + 1 && ($rep_scheme[$p] != $rep_scheme[$k]  || $percentage_scheme[$p] != $percentage_scheme[$k])){
									$movement_str .= $rep_scheme[$p] . " @ " . $percentage_scheme[$p] . "%, ";
									break;
								}
							
								// This means we found some that are the same
								if($k == sizeof($percentage_scheme) - 1){
									$num_sets = $k + 1 - $p;
								}
								else {
									$num_sets = $k - $p;
								}
								$movement_str .= $num_sets . "x" . $rep_scheme[$p] . " @ " . $percentage_scheme[$p] . "%, ";
								if($k == sizeof($percentage_scheme)-1){
									$p = $k;
								}
								else {
									$p = $k - 1;
								}
								break;
							}
						}
						else {
							if( ($percentage_scheme[$p] != $percentage_scheme[$k] || 
									($percentage_scheme[$p] == $percentage_scheme[$k] && $k == sizeof($percentage_scheme) - 1))
							){
								if($k == $p + 1 && ($percentage_scheme[$p] != $percentage_scheme[$k])){
									$movement_str .= $rep_scheme[$p] . " @ " . $percentage_scheme[$p] . "%, ";
									break;
								}
							
								// This means we found some that are the same
								if($k == sizeof($percentage_scheme) - 1){
									$num_sets = $k + 1 - $p;
								}
								else {
									$num_sets = $k - $p;
								}
								$movement_str .= $num_sets . "x" . $rep_scheme[$p] . " @ " . $percentage_scheme[$p] . "%, ";
								if($k == sizeof($percentage_scheme)-1){
									$p = $k;
								}
								else {
									$p = $k - 1;
								}
								break;
							}
						}
					}
				}

				$movement_str .= $workout[0]->movement;
			}

			else{
				$movement_str .= $workout[0]->rep_scheme . ',';
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
			}

			if(isset($workout[0]->clock)){
				if(strpos($workout[0]->clock, ":") !== FALSE){
					$movement_str = $movement_str . ' on a ' . $workout[0]->clock . ' clock';
				}
				else {
					$movement_str = $movement_str . ' on a ' . $workout[0]->clock . ':00 clock';
				}
			}
		}
	}
	return $movement_str;
}

function create_conditioning_string($wid){

	$conditioning = db_query('SELECT * FROM {workout_builder_conditioning} where wid=:wid', array(':wid' => $wid));
	$conditioning_notes_results = db_query('SELECT * FROM {workout_builder_conditioning_notes} where wid=:wid', array(':wid' => $wid));

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
		}
	}
	return $movement_str;
}
