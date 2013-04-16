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
	width:128px;
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

	//include 'C:\wamp\www\drupal\sites\all\modules\workout_results\workout_results.inc';
	include '/var/www/includes/utils.inc';

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
				$picture_uri = db_query("SELECT uri FROM {file_managed} where uid=:uid", array(':uid' => $user->uid))->fetchField();
				if($picture_uri != NULL){
					print '<img src="' . file_create_url($picture_uri) . '">';			
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
						if($currwid == $wid && $wid != NULL){
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
