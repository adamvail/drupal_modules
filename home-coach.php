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
	width:60%
}
#hist {
	clear:both;
	margin:10px;
}
#prpic {
	float:right;
	border-style:solid;
	border-width:2px;
	border-color:#D4D4D4;
	border-radius: 10px;
}
td{
	text-align:center;
}
.centered-cell {
  text-align: center;
} 
body {
	font-family:Verdana;
}
</style>




<?php

	include 'C:\wamp\www\drupal\sites\all\modules\workout_results\workout_results.inc';
	//include '/var/www/includes/utils.inc';

	global $user;

	if ( $user->uid ) {
		print '<div id="prof">';
		
			$result = db_query('SELECT * FROM {workout_gym_affiliation} WHERE uid = :uid', array(':uid' => $user->uid));
			foreach($result as $usr){
				print '<h1 class="centered-cell"> User Profile </h1>';
				print 'User: ' . $usr->name;
				print '<br>';
				print 'Role: ' . $usr->role;
				print '<br>';
				print 'Gym: ' . $usr->gym;
			}
			
			print '<div id="prpic">';
				print '<img src="' . base_path() . drupal_get_path('theme', 'skeletontheme') . '/mockup/user.png">';
			print '</div>'; //end prpic div
			
		print '</div>'; //end prof div
		
		print '<div id="wod">';

		//build the wod table
		print '<table>';
		
			print '<tr>';
				$wid = db_query('SELECT MAX(wid) FROM {workout_builder_strength} WHERE creator_id = :uid', array(':uid' => $user->uid))->fetchField();
				print '<th colspan="0" class="centered-cell"><a  href="?q=workout_tracker&wid=' . (int)$wid . '">' . 'Workout of the Day' . '</a></th>';	
			print '</tr>';
			
			//strength portion of the wod
			print '<tr>';
				print '<th colspan="0">' . 'Strength Portion:' . '</th>';	
			print '</tr>';
			
			//gather the strength portion of the last built workout for gym user is in
			db_query('SELECT @last_id := MAX(wid) FROM {workout_builder_strength} WHERE creator_id = :uid', array(':uid' => $user->uid));
			$ret = db_query('SELECT * FROM {workout_builder_strength} WHERE wid = @last_id');
			//print out the strength portion of the workout
			foreach($ret as $workout){
				$output = create_strength_string($workout->wid);
				print '<tr>';
					print '<td>' . $output . '</td>';
				print '</tr>';
			}
			
			//conditioning portion of the wod
			print '<tr>';
				print '<th colspan="0">' . 'Conditioning Portion:' . '</th>';	
			print '</tr>';
			$ret = db_query('SELECT * FROM {workout_builder_conditioning} WHERE wid = @last_id');
			//print out the conditioning portion of the workout
			foreach($ret as $workout){
				$output = create_conditioning_string($workout->wid);
				print '<tr>';
					print '<td>' . $output . '</td>';
				print '</tr>';
			}
		print '</table>';		
	
		print '</div>'; //end wod div
		
		print '<hr>';
		
		print '<div id="hist">';
		
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
					
						//print gym member names
						print '<td>' . $usr->name . '</td>';
						
						//print whether a workout has been completed or not
						$auid = db_query('SELECT uid FROM {workout_gym_affiliation} WHERE name LIKE :name', array(':name' => $usr->name))->fetchField();
						$currwid = db_query('SELECT MAX(wid) FROM {workout_tracker_strength} WHERE athlete_uid = :auid', array(':auid' => $auid))->fetchField();
						$lwid = db_query('SELECT MAX(wid) FROM {workout_builder_strength} WHERE creator_id = :myuid', array(':myuid' => $user->uid))->fetchField();
						if($currwid == $lwid && $lwid != NULL){
							print '<td>Completed</td>';
						}
						else{
							print '<td></td>';
						}						
					print '</tr>';
				}		
			print '</table>';
		print '</div>'; //end hist div
	}
?>

</body>
</html>
