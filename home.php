<!DOCTYPE html>
<html>
<body>

<style>
/* max_reps style sheet */
#wod{
	margin:10px;
	width:100%;
}
#one_half{
	float:left;
	width:40%;
	margin:20px;
}
#two_half{
	float:left;
	padding: 0px 20px 0px 20px;
	width:45%;
}
#title{
	text-align:center;
}
th,td{
	text-align:center;
}
</style>




<?php
	global $user;

	if ( $user->uid ) {
		print '<div id="one_half">';
			print '<img src="' .  base_path() . drupal_get_path('theme', 'skeletontheme') . '/mockup/workout-motivation.jpg">';
		print '</div>';

		print '<div id="two_half">';
	
			db_query('SELECT @last_id := MAX(wid) FROM {workout_builder_strength}');
			$result = db_query('SELECT * FROM {workout_builder_strength} WHERE wid = @last_id');
			print '<div id="wod">';
				print '<table>';
				
					print '<tr>';
						print '<th colspan="4">' . 'Workout of the Day:' . '</th>';	
					print '</tr>';
					
					print '<tr>' ;
						print '<th>' . 'Sets' . '</th>';
						print '<th>' . 'Reps' . '</th>';
						print '<th>' . 'Movement' . '</th>';
						print '<th>' . 'Weight (%)' . '</th>';
					print '</tr>';
					foreach ($result as $row) {
						print '<tr>' ;
							print '<td>' . $row->sets . '</td>';
							print '<td>' . $row->reps . '</td>';
							print '<td>' . $row->movement . '</td>';
							print '<td>' . $row->wgt_percentage . '</td>';
						print '</tr>';
					}
				print '</table>';

	}
?>

</body>
</html>