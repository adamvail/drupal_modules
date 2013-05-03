<!DOCTYPE html>
<html>
<head>

<script src="//ajax.googleapis.com/ajax/libs/jquery/1.9.1/jquery.min.js"></script>

<script type="text/javascript">

	function toggle_visibility(id) {
		var xmlhttp;
		if (window.XMLHttpRequest)
		{// code for IE7+, Firefox, Chrome, Opera, Safari
			xmlhttp=new XMLHttpRequest();
		}
		else
		{// code for IE6, IE5
			xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");
		}
		
		xmlhttp.onreadystatechange=function()
		{
			if (xmlhttp.readyState==4 && xmlhttp.status==200)
			{
				
				var e = document.getElementById(id);
				if(e.style.display == 'block') {
					e.style.display = 'none';
				}
				else {
					document.getElementByClassName("hidden-prof").style.display = 'none';
					e.style.display = 'block';
				}
			}
		}
		xmlhttp.open("GET","http://localhost/drupal/?q=members",true);
		xmlhttp.send();
    }

</script>

<script>
	function hideallexcept(className, must_show) {
		var els = document.getElementsByTagName('div')
		for (i=0;i<els.length; i++) {
			if (els.item(i).className == className){
				els.item(i).style.display="none";
			}
		}
		document.getElementById(must_show).style.display='block';
	}
</script>

<script type="text/javascript">

  var _gaq = _gaq || [];
  _gaq.push(['_setAccount', 'UA-40201944-1']);
  _gaq.push(['_trackPageview']);

  (function() {
    var ga = document.createElement('script'); ga.type = 'text/javascript'; ga.async = true;
    ga.src = ('https:' == document.location.protocol ? 'https://ssl' : 'http://www') + '.google-analytics.com/ga.js';
    var s = document.getElementsByTagName('script')[0]; s.parentNode.insertBefore(ga, s);
  })();

</script>

</head>

<body>

<style>
/* members style sheet */

#members {
	width:50%;
	margin:10px;
	float:left;
}
#prof {
	border-style:solid;
	border-width:2px;
	border-color:#D4D4D4;
	border-radius:10px;
	float:right;
	padding:20px;
	margin:5px;
	width:70%;
}
#prpic {
	float:right;
	border-style:solid;
	border-width:2px;
	border-color:#D4D4D4;
	width:40%;
}
#prpic img {
	max-width:100%; 
	max-height:100%;
	margin:auto;
	display:block;
}
#uname {
	margin:0px 0px 0px 20px;
}
#stats {
	float:left;
	width:40%;
}	
.centered-cell {
  text-align:center;
}
.hidden-prof {
	display:none;
	float:right;
	width:45%;
}
.hidden-results {
	display:none;
	clear:both;
	padding:40px;
}
</style>


					
	<?php
		
		//check include statements
		//if(!@include '/var/www/sites/all/modules/workout_results/workout_results.inc') {
			//include 'C:/wamp/www/drupal/sites/all/modules/workout_results/workout_results.inc';
			//render_page();
		//}
		//else {
			render_page();
		//}
		
		function render_page() {
			
			global $user;
			$cindex = 0;
		
			print '<div id="members">';
			
				print '<table>';
				
					$gym = db_query('SELECT gym FROM {workout_gym_affiliation} WHERE uid =:uid', array(':uid' => $user->uid))->fetchField();
					$result = db_query('SELECT * FROM {workout_gym_affiliation} WHERE gym LIKE :mygym', array(':mygym' => $gym));
					
					print '<tr><th colspan="0" class="centered-cell">' . pretty_print($gym) . ' Members </th></tr>'; 
					
					$members;
					foreach($result as $member){
						// display their name and role in the member list
						print '<tr><td><a href="/drupal/#" onclick="hideallexcept(\'hidden-prof\',\'memberprof' . $cindex . '\'); hideallexcept(\'hidden-results\', \'memberresults' . $cindex . '\'); return false;">' . pretty_print($member->name) .'</a></td><td>' . pretty_print($member->role) . '</td></tr>';
						$members[$cindex] = $member->uid;
						$cindex++;
						
					}
				
				print '</table>';
			print '</div>'; //end members div
			
			//display a members profile\
			for($i=0; $i<$cindex; $i++){
				print '<div class="hidden-prof" id="memberprof' . $i . '">';
					print '<div id="prof">';
				
						//$mygym = '';
						$result = db_query('SELECT * FROM {workout_gym_affiliation} WHERE uid = :uid', array(':uid' => $members[$i]));
						foreach($result as $usr){
							//name and gym affiliation section
							print '<div id="uname">';
								print '<p id="uname"><font color="179ce8" size="5"> ' . pretty_print($usr->name) . '</font>';
								print '<br>';
								print '<em>' . pretty_print($usr->gym) . '</em></p>';
								print '<hr>';
							print '</div>'; //end uname div
							
							$twods = db_query('SELECT COUNT(DISTINCT wid) FROM {workout_tracker_strength} WHERE athlete_uid = :myuid', array(':myuid'=> $members[$i]))->fetchField();
							$weight_lb = db_query('SELECT SUM(work) FROM {workout_tracker_strength} WHERE athlete_uid = :myuid AND weight_units=:units', array(':myuid'=> $members[$i], ':units' => 0))->fetchField();
							$weight_kg = db_query('SELECT SUM(work) FROM {workout_tracker_strength} WHERE athlete_uid = :myuid AND weight_units=:units', array(':myuid'=> $members[$i], ':units' => 1))->fetchField();
							
							$tweight = $weight_lb + ($weight_kg * 2.2);
							
							print '<div class="centered-cell" id="stats">';
								print '<table>';
									print '<tr><th>' . pretty_print($usr->role) . ' Stats:</th></tr>';
									print '<tr><td><font size="2"><b>' . (int)$twods . '</b></font></td></tr>';
									print '<tr><td><font color="179ce8" size="1"> WODs Complete </font></td></tr>';
									print '<tr><td><font size="2"><b>' . $tweight . ' lbs</b></font></td></tr>'; 
									print '<tr><td><font color="179ce8" size="1"> Weight Lifted </font></td></tr>';
								print '</table>';
							print '</div>'; //end stats div
							//$mygym = $usr->gym;
						}
						
						print '<div id="prpic">';
							$picture_uri = db_query("SELECT uri FROM {file_managed} where uid=:uid", array(':uid' => $members[$i]))->fetchField();
							if($picture_uri != NULL){
								print '<img src="' . file_create_url($picture_uri) . '">';			
							}
							else {		
								print '<img src="' . base_path() . 'misc/druplicon.png">';
							}
						print '</div>'; //end prpic div
						
					print '</div>'; //end prof div
				print '</div>';
			}
			
			//display a members last completed workout\
			for($i=0; $i<$cindex; $i++){
				print '<div class="hidden-results" id="memberresults' . $i . '">';
				
					
				
					//logic to get the most recent WOD which can be either from the coach or the athlete
					$wid = 0;	
					// grab the most up to date wid the athlete has built
					$swida = db_query('SELECT MAX(wid) FROM {workout_tracker_strength} WHERE athlete_uid = :auid and date < :date', array(':auid' => $members[$i], ':date' => time()))->fetchField();
					$cwida = db_query('SELECT MAX(wid) FROM {workout_tracker_conditioning} WHERE athlete_uid = :auid and date < :date', array(':auid' => $members[$i], ':date' => time()))->fetchField();
					
					$strength_str = NULL;
					$conditioning_str = NULL;
					
					
					
					if($swida > $cwida) {
						
						$wid = $swida;
						print '1 Member id: ' . $members[$i] . ' wid: ' . $wid . '<br>';
						$strength_str = create_strength_string($wid, 1, $members[$i]);
					}
					elseif($cwida > $swida) {
						
						$wid = $cwida;
						print '2 Member id: ' . $members[$i] . ' wid: ' . $wid . '<br>';
						$conditioning_str = create_conditioning_string($wid, 1);
					}
					elseif($swida == $cwida && $swida != 0) {
						
						$wid = $swida;
						print '3 Member id: ' . $members[$i] . ' wid: ' . $wid . '<br>';
						$strength_str = create_strength_string($wid, 1, $members[$i]);
						$conditioning_str = create_conditioning_string($wid, 1);
					}
					
					if(!empty($strength_str) || !empty($conditioning_str)) {
						print '<table>';
							
							print '<tr><th colspan="0" class="centered-cell"> Most Recent WOD </th></tr>';
							if(!empty($strength_str)) {
								print '<tr><th colspan="0"> Strength Portion </th></tr>';
								print '<tr><td>' . $strength_str . '<td><tr>';
							}
							if(!empty($conditioning_str)) {
								print '<tr><th colspan="0"> Conditioning Portion </th></tr>';
								print '<tr><td>' . $conditioning_str . '<td><tr>';
							}
							
						print '</table>';
					}
				print '</div>';
			}
		}
		
		//helper print function
		function pretty_print($str){
			$pretty_str = implode(' ', array_map('ucfirst', explode(' ', $str)));
			return $pretty_str;
		}
		
		function convert_timestamp($timestamp){
			$format = 'l, n/j/Y';
			return date($format, $timestamp);
		}
		
		function create_conditioning_string($wid, $num_performed){

			$workouts = db_select('workout_builder_conditioning', 'b')
				->fields('b')
				->condition('wid', $wid, '=')
				->execute()
				->fetchAll();
			
			$notes = db_select('workout_builder_conditioning_notes', 'n')
				->fields('n')
				->condition('wid', $wid, '=')
				->execute()
				->fetchAll();

			$logs = db_select('workout_tracker_conditioning', 'c')
				->fields('c')
				->condition('wid', $wid, '=')
				->condition('num_performed', $num_performed, '=')
				->execute()
				->fetchAll();

			$headers = array();
			foreach($workouts as $w){
				$headers[$w->conditioning_id] = $w->workout;
			}
			foreach($notes as $n){
				$headers[$n->conditioning_id] .= "<br><br>" . $n->notes;
			}

			$results = '';
			foreach($logs as $l){
				$results .= $headers[$l->cid] . "<br><br>"; 
				$lines = explode("\n", $l->results);
				foreach($lines as $line){
					$results .= $line . "<br>"; 
				}
				$results .= "<br>";  
			}
			return $results;
		}
		
		function create_strength_string($wid, $num_performed, $uid){

			$headers = assemble_strength_headers($wid);
			$sids = array_keys($headers);
			for($i = 0; $i < sizeof($sids); $i++){
				$workout_result = '';
				$sid = $sids[$i];
				$results_headers = create_results_array($wid, $sid);
				$strength_results = fillin_strength_results($results_headers, $wid, $sid, $num_performed, $uid);
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

		function fillin_strength_results($headers, $wid, $sid, $num_performed, $uid){
			

			$strength_results = db_query('SELECT * from {workout_tracker_strength} where wid=:wid AND athlete_uid=:aid AND sid=:sid AND num_performed=:np', array(':wid' => $wid, ':aid' => $uid, ':sid' => $sid, ':np' => $num_performed))->fetchAll();

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
		
		function pretty_print_movement($movement){
			$movement_pretty = implode(' ', array_map('ucfirst', explode(' ', $movement)));
			$movement_pretty = implode('-', array_map('ucfirst', explode('-', $movement_pretty)));
			$movement_pretty = implode('[', array_map('ucfirst', explode('[', $movement_pretty)));
			$movement_pretty = implode(']', array_map('ucfirst', explode(']', $movement_pretty)));
			$movement_pretty = implode('+', array_map('ucfirst', explode('+', $movement_pretty)));
			return $movement_pretty;
		}
		
		
	?>
	
</body>

</html>
