<!DOCTYPE html>
<html>
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
	border-radius: 10px;
	float:left;
	width:30%;
	padding:20px;
	margin:5px;
}
#wod {
	float:right;
	/*border-style:solid;
	border-width:2px;
	border-color:grey;*/
	margin:5px;
	width:60%;
}
#hist {
	clear:both;
	float:left;
	margin:5px;
	width:50%;
}
#graph {
	float:right;
	margin:5px;
	//border-style:solid;
	//border-width:2px;
	//border-color:#D4D4D4;
	//border-radius: 10px;
	padding:20px;
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
td{
	text-align:center;
}
.centered-cell {
  text-align: center;
} 
</style>




<?php
//	include 'C:\wamp\www\drupal\sites\all\modules\workout_results\workout_results.inc';
	include '/var/www/sites/all/modules/workout_results/workout_results.inc';


	global $user;

	if ( $user->uid ) {
		print '<div id="prof">';
			
			//print out the user profile information
			$result = db_query('SELECT * FROM {workout_gym_affiliation} WHERE uid = :uid', array(':uid' => $user->uid));
			$mygym = '';
			foreach($result as $usr){
				print '<h1 class="centered-cell"> User Profile </h1>';
				print 'User: ' . $usr->name;
				print '<br>';
				print 'Role: ' . $usr->role;
				print '<br>';
				print 'Gym: ' . $usr->gym;
				$mygym = $usr->gym;
			}
			
			//disply the users profile picture
			print '<div id="prpic">';
				$picture_uri = db_query("SELECT uri FROM {file_managed} where uid=:uid", array(':uid' => $user->uid))->fetchField();
				if($picture_uri != NULL){
					print '<img src="' . file_create_url($picture_uri) . '">';			
				}
				else {		
					print '<a href="?q=user/' . $user->uid . '/edit"><img src="' . base_path() . 'misc/druplicon.png"></a>';
				}
			print '</div>'; //end prpic div
			
		print '</div>'; //end prof div
		
		
		
		//logic to get the most recent WOD which can be either from the coach or the athlete
		$wid = 0;	
		// grab the most up to date wid the athlete has built
		$swida = db_query('SELECT MAX(wid) FROM {workout_builder_strength} WHERE creator_id = :auid', array(':auid' => $user->uid))->fetchField();
		$cwida = db_query('SELECT MAX(wid) FROM {workout_builder_conditioning} WHERE creator_id = :auid', array(':auid' => $user->uid))->fetchField();
		if($swida > $cwida) {
			$wid = $swida;
		}
		else {
			$wid = $cwida;
		}
		//grab the coaches uid
		$cuid = db_query('SELECT uid FROM {workout_gym_affiliation} WHERE gym LIKE :mygym AND role LIKE :role', array(':mygym' => $mygym, ':role' => 'coach'))->fetchField();
		// grab the most up to date wid the coach has built
		$swidc = db_query('SELECT MAX(wid) FROM {workout_builder_strength} WHERE creator_id = :cuid', array(':cuid' => $cuid))->fetchField();
		$cwidc = db_query('SELECT MAX(wid) FROM {workout_builder_conditioning} WHERE creator_id = :cuid', array(':cuid' => $cuid))->fetchField();
		if($swidc > $wid) {
			$wid = $swidc;
		}
		if ($cwidc > $wid) {
			$wid = $cwidc;
		}
		
		//build the workout of the day
		print '<div id="wod">';
	
			print '<table>';	
				//print the table header
				print '<tr>';
					print '<th colspan="0" class="centered-cell"><a href="?q=workout_tracker&wid=' . (int)$wid . '">' . 'Workout of the Day' . '</a></th>';	
				print '</tr>';
				
				//print that no workouts exist and one should be created.
				if($wid == 0) {
					print '<tr>';
						print '<td> No workouts exist: Use workout builder to create one.</td>'; 
					print '</tr>';
				}
				//a workout exists, print it out
				else {
					//strength portion of the wod
					if($wid == $swida || $wid == $swidc) {
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
					if($wid == $cwida || $wid == $cwidc) {
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
		
		//render the workout results form to the page
		print '<div id="hist">';
			print '<table>';
				print '<tr>';
					print '<th colspan="0" class="centered-cell"> WOD History </th>'; 
				print '</tr>';
				print '<tr>';
					print '<td>' . drupal_render(drupal_get_form('workout_results')) . '</td>'; 
				print '</tr>';
			print '</table>';
		print '</div>'; //end hist div
		
		print '<div id="graph">';
			//*-------------------------------------------------
			
			$chart = array(
				'#chart_id' => 'weight_chart',
				'#title' => chart_title(t('Weight Lifted per Movement'), 15),
				'#type' => CHART_TYPE_BAR_V_GROUPED,
				'#size' => chart_size(300, 200),
				'#adjust_resolution' => TRUE,
				'#grid_lines' => chart_grid_lines(10, 20), 
				'#bar_size' => chart_bar_size(35, 15), 
			);
			
			//gather data for the chart
			$chart['#data']['clean'] = array(1);
			$chart['#data']['jerk']  = array(2);
			$chart['#data']['squat']  = array(5);
			$chart['#data']['Deadlift']  = array(2);

			//create the legend
			$chart['#legends'][] = t('Clean');
			$chart['#legends'][] = t('Jerk');
			$chart['#legends'][] = t('Squat');
			$chart['#legends'][] = t('Deadlift');

			//set chart bar line colors
			$chart['#data_colors'][] = '00ff00';
			$chart['#data_colors'][] = 'ff0000';
			$chart['#data_colors'][] = '0000ff';
			$chart['#data_colors'][] = 'FBEC5D';
			
			$chart['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][0][] = chart_mixed_axis_range_label(0, 5);
			$chart['#mixed_axis_labels'][CHART_AXIS_Y_LEFT][1][] = chart_mixed_axis_label(t('Weight'), 95);

			
			$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t(''));
			//$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('Jerk'));
			//$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('Squat'));
			//$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][1][] = chart_mixed_axis_label(t('Deadlift'));  
			$chart['#mixed_axis_labels'][CHART_AXIS_X_BOTTOM][2][] = chart_mixed_axis_label(t('Movement'), 50);


			print theme('chart', array('chart' => $chart));

			//--------------------------------------------------*/
		print '</div>'; //end graph div
	}
?>

</body>
</html>
