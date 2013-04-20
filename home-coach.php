<!DOCTYPE html>
<html>
<head>
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
				if(e.style.display == 'block')
					e.style.display = 'none';
				else
					e.style.display = 'block';
			}
		}
		xmlhttp.open("GET","http://localhost/drupal/",true);
		xmlhttp.send();
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
/* max_reps style sheet */

#title {
	text-align:center;
}
#prof {
	border-style:solid;
	border-width:2px;
	border-color:#D4D4D4;
	border-radius:10px;
	float:left;
	width:30%;
	padding:15px;
	margin:5px;
}
#wod {
	float:right;
	margin:5px;
	width:60%
}
#members {
	clear:both;
	float:left;
	width:40%;
	margin:10px;
}
#prpic {
	float:right;
	border-style:solid;
	border-width:2px;
	border-color:#D4D4D4;
	width:40%;
}
.invisgraph {
	float:right;
	margin:5px;
	border-style:solid;
	border-width:2px;
	border-color:#D4D4D4;
	border-radius: 10px;
	padding:20px;
	display:none;
	width:50%;
	text-align:center;
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
td{
	text-align:center;
}
.centered-cell {
  text-align: center;
} 
</style>


<?php

	//check include statements
	if(!@include '/var/www/sites/all/modules/workout_results/workout_results.inc') {
		include 'C:/wamp/www/drupal/sites/all/modules/workout_results/workout_results.inc';
		render_page();
	}
	else {
		render_page();
	}
	
	
	function render_page(){
		global $user;
		$graphs = array();
		$cindex = 0;

		if ( $user->uid ) {
			print '<div id="prof">';
			
				$result = db_query('SELECT * FROM {workout_gym_affiliation} WHERE uid = :uid', array(':uid' => $user->uid));
				foreach($result as $usr){
					//name and gym affiliation section
					print '<div id="uname">';
						print '<p id="uname"><a href="?q=user/' . $user->uid . '/edit" style="text-decoration: none"><font color="179ce8" size="5"> ' . pretty_print($usr->name) . '</a></font>';
						print '<br>';
						print '<em>' . pretty_print($usr->gym) . '</em></p>';
						print '<hr>';
					print '</div>'; //end uname div
					
					$twods = db_query('SELECT COUNT(DISTINCT wid) FROM {workout_tracker_strength} WHERE athlete_uid = :myuid', array(':myuid'=> $usr->uid))->fetchField();
					$nmembers = db_query('SELECT COUNT(DISTINCT uid) FROM {workout_gym_affiliation} WHERE gym LIKE :mygym', array(':mygym'=> $usr->gym))->fetchField();
					
					
					print '<div class="centered-cell" id="stats">';
						print '<table>';
							print '<tr><th>' . pretty_print($usr->role) . ' Stats:</th></tr>';
							print '<tr><td><font size="2"><b>' . $twods . '</b></font></td></tr>';
							print '<tr><td><font color="179ce8" size="1"> WODs Complete </font></td></tr>';
							print '<tr><td><font size="2"><b>' . $nmembers . '</b></font></td></tr>';
							print '<tr><td><font color="179ce8" size="1"> Gym Members </font></td></tr>';
						print '</table>';
					print '</div>'; //end stats div
				}
				
				print '<div id="prpic">';
					$picture_uri = db_query("SELECT uri FROM {file_managed} where uid=:uid", array(':uid' => $user->uid))->fetchField();
					if($picture_uri != NULL){
						print '<a href="?q=user/' . $user->uid . '/edit"><img src="' . file_create_url($picture_uri) . '">';		
					}
					else {		
						print '<a href="?q=user/' . $user->uid . '/edit"><img src="' . base_path() . 'misc/druplicon.png"></a>';
					}
				print '</div>'; //end prpic div
				
			print '</div>'; //end prof div
			
			print '<div id="wod">';

			//build the wod table
			print '<table>';
			
				print '<tr>';
					$wid = 0;
					$swid = db_query('SELECT MAX(wid) FROM {workout_builder_strength} WHERE creator_id = :uid', array(':uid' => $user->uid))->fetchField();
					$cwid = db_query('SELECT MAX(wid) FROM {workout_builder_conditioning} WHERE creator_id = :uid', array(':uid' => $user->uid))->fetchField();
					//print (int)$swid . ' ' . (int)$cwid;
					if((int)$swid != 0 || (int)$cwid != 0) {
						if($swid > $cwid) {
							//print 'here';
							$wid = $swid;
						}
						else {
							//print 'here1';
							$wid = $cwid;
						}
					}

					print '<th colspan="0" class="centered-cell"><a  href="?q=workout_tracker&wid=' . (int)$wid . '">' . 'Workout of the Day' . '</a></th>';	
				print '</tr>';
				
				//strength portion of the wod
				if($wid == 0) {
					print '<tr>';			
								print '<td> No workouts exist: Use workout builder to create one.</td>';					
					print '</tr>';
				}
				else {
					//strength portion of the wod
					if($wid == $swid) {
						print '<tr>';			
								print '<th colspan="0">' . 'Strength Portion:' . '</th>';					
						print '</tr>';
						
						//print out the strength portion of the workout
						$output = assemble_strength_headers($wid);
						//print sizeof($output);
						for ($i=1; $i<=sizeof($output); $i++) {
							print '<tr>';
								print '<td>' . $output[$i] . '</td>';
							print '</tr>';
						}
					}
					
					
					//conditioning portion of the wod
					if($wid == $cwid) {
						print '<tr>';
							print '<th colspan="0">' . 'Conditioning Portion:' . '</th>';	
						print '</tr>';
						
						//print out the conditioning portion of the workout
						$output = assemble_conditioning_headers($wid);
						for ($i=1; $i<=sizeof($output); $i++) {
							print '<tr>';
								print '<td>' . $output[$i] . '</td>';
							print '</tr>';
						}			
					}
				}
			print '</table>';		
		
			print '</div>'; //end wod div
			
			print '<hr>';
			
			print '<div id="members">';
			
				print '<table>';
				
					//member table header
					print '<tr>';
						print '<th colspan="0" class="centered-cell">Gym Members</th>';
					print '</tr>';
					
					//member name and WOD complete header
					print '<tr>';
						print '<th class="centered-cell">Member Name</th>';
						print '<th class="centered-cell">WOD Completed</th>';
					print '</tr>';
					
					
					$mygym = db_query('SELECT gym FROM {workout_gym_affiliation} WHERE uid = :uid', array(':uid' => $user->uid))->fetchField();
					$ret = db_query('SELECT name FROM {workout_gym_affiliation} WHERE gym LIKE :mygym', array(':mygym' => $mygym));
					foreach($ret as $usr){
						// print the content of the table
						print '<tr>';	
						
							$auid = db_query('SELECT uid FROM {workout_gym_affiliation} WHERE name LIKE :name', array(':name' => $usr->name))->fetchField();
							$currwid = db_query('SELECT MAX(wid) FROM {workout_tracker_strength} WHERE athlete_uid = :auid', array(':auid' => $auid))->fetchField();
						
							if($currwid == $wid && $wid != NULL){
								//print clickable gym member name
								print '<td><a href="/drupal/#" onclick="toggle_visibility(\'graph' . $cindex . '\'); return false;">' . pretty_print($usr->name) . '</a></td>';
								//print whether a workout has been completed or not
								print '<td>Completed</td>';
								
								//build their graph
								$chart = array(
									'#chart_id' => 'wod_chart' . $cindex,
									'#title' => chart_title(t('WOD Breakdown'), 15),
									'#type' => CHART_TYPE_BAR_V_GROUPED,
									'#size' => chart_size(400, 200),
									'#adjust_resolution' => TRUE, 
									'#bar_size' => chart_bar_size(15, 5), 
								);
								
								$workout = db_query('SELECT * FROM {workout_tracker_strength} WHERE wid = :wid AND athlete_uid = :auid', array(':wid' => $wid, ':auid' => $auid));
								$exercise = db_query('SELECT movement FROM {workout_builder_strength} WHERE creator_id = :uid AND wid = :wid', array(':uid' => $user->uid,':wid'=>$wid));
								$size = db_query('SELECT sid FROM {workout_tracker_strength} WHERE wid = :wid AND athlete_uid = :auid ORDER BY sid DESC LIMIT 1', array(':wid' => $wid, ':auid' => $auid))->fetchField();
								//initialize data array
								if(empty($size)){
									return;
								}
								$weights = array_fill(1, $size, 0);
								
								//sum the weight lifted for each movement
								foreach($workout as $move) {
									$weights[$move->sid] += $move->work;					
								}

								//create the data arrays
								for($i=1; $i<=sizeof($weights); $i++) {
									$chart['#data'][$i] = array($weights[$i]);
									$chart['#data_colors'][] = chart_unique_color('$chart[\'#data\'][' . $i . ']');
								}
								//create the workout legend
								foreach($exercise as $wmove) {
									$chart['#legends'][] = t(pretty_print($wmove->movement));					
								}

								//create chart axis labels and tick ranges
								$chart['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][0][] = chart_mixed_axis_range_label(0,max($weights));
								$chart['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][1][] = chart_mixed_axis_label(t('Weight'), 50);
								
								$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t(''));
								$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][2][] = chart_mixed_axis_label(t('Movements'), 50);
								
								//store the graph
								$graphs[$cindex] = $chart;
								$cindex++;
							}
							else{
								//print nonclickable gym member name
								print '<td>' . pretty_print($usr->name) . '</td>';
								//workout not complete
								print '<td></td>';
							}						
						print '</tr>';
					}		
				print '</table>';
			print '</div>'; //end members div
			
			for($i=0; $i<$cindex; $i++){
				print '<div class="invisgraph" id="graph' . $i . '">';
					print theme('chart', array('chart' => $graphs[$i]));
				print '</div>'; //end graph div
			}
		}
	}
	
	//helper print function
	function pretty_print($str){
		$pretty_str = implode(' ', array_map('ucfirst', explode(' ', $str)));
		return $pretty_str;
	}
	
?>

</body>
</html>
