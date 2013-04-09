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
	//require_once '/var/www/includes/utils.inc';
	require_once '/var/www/sites/all/modules/workout_results/workout_results.inc';
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
		$result = db_query('SELECT gym FROM {workout_gym_affiliation} WHERE uid = :uid', array(':uid' => $user->uid));
		foreach($result as $name){
			//build the wod table
			print '<table>';
				print '<tr>';
					$wid = db_query('SELECT MAX(wid) FROM {workout_builder_conditioning}')->fetchField();
					print '<th colspan="0" class="centered-cell"><a  href="?q=workout_tracker&wid=' . (int)$wid . '">' . 'Workout of the Day' . '</a></th>';	
				print '</tr>';
				//strength portion of the wod
				print '<tr>';
					print '<th colspan="0">' . 'Strength Portion:' . '</th>';	
				print '</tr>';
				//gather the strength portion of the last built workout for gym user is in
				db_query('SELECT @last_id := MAX(wid) FROM {workout_builder_strength}');
				$ret = db_query('SELECT * FROM {workout_builder_strength} WHERE wid = @last_id AND gym LIKE :gym', array(':gym' => $name->gym));
				//print out the strength portion of the workout
				foreach($ret as $row){
					//$output = create_strength_string($row->wid);
					print '<tr>';
						//print '<td>' . $output . '</td>';
					print '</tr>';
				}
				//conditioning portion of the wod
				print '<tr>';
					print '<th colspan="0">' . 'Conditioning Portion:' . '</th>';	
				print '</tr>';
				$ret = db_query('SELECT * FROM {workout_builder_conditioning} WHERE wid = @last_id AND gym LIKE :gym', array(':gym' => $name->gym));
				//print out the conditioning portion of the workout
				foreach($ret as $row){
					//$output = create_conditioning_string($row->wid);
					print '<tr>';
						//print '<td>' . $output . '</td>';
					print '</tr>';
				}
			print '</table>';		
		}
		print '</div>'; //end wod div
		print '<hr>';
		print '<div id="hist">';
			print '<table>';
				print '<tr>';
					print '<th colspan="0" class="centered-cell"> WOD History </th>'; 
				print '</tr>';
				print '<tr>';
					print '<td>' . drupal_render(drupal_get_form('workout_results')) . '</td>'; 
				print '</tr>';
			print '</table>';
		print '</div>'; //end prev div
	}
?>

</body>
</html>
