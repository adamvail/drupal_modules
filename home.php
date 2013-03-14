<!DOCTYPE html>
<html>
<body>

<style>
/* max_reps style sheet */
#wod{
	margin:10px;
	width:100%;
	clear:both;
}
#cond{
	margin:10px;
	width:100%;
	clear:both;
}
#one_half{
	float:left;
	width:40%;
	margin:20px;
	border-style:solid;
	border-width:5px;
}
#two_half{
	float:left;
	padding: 0px 20px 0px 20px;
	border-style:solid;
	border-width:5px;
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
		print '<div class="one_half">';
			print '<img src="' .  base_path() . drupal_get_path('theme', 'skeletontheme') . '/mockup/workout-motivation.jpg">';
			db_query('SELECT @last_id := MAX(wid) FROM {workout_builder_conditioning}');
			$result = db_query('SELECT * FROM {workout_builder_conditioning} WHERE wid = @last_id');
			$query = db_query('SELECT wid FROM {workout_builder_conditioning} WHERE wid = @last_id');
			$wid = 0;
			foreach($query as $row){
				$wid = $row->wid;
			}
				print '<table>';
				
					print '<tr>';
						print '<th colspan="4"><a href="?q=workout_tracker&wid=' . $wid . '">' . 'Conditioning Portion:' . '</a></th>';	
					print '</tr>';
					
					print '<tr>' ;
						print '<th>' . 'Style' . '</th>';
						print '<th>' . 'Duration' . '</th>';
						print '<th>' . 'Reps' . '</th>';
						print '<th>' . 'Movement' . '</th>';
					print '</tr>';
					foreach ($result as $row) {
						print '<tr>' ;
							print '<td>' . $row->wid . '</td>';
							print '<td>' . $row->duration . '</td>';
							print '<td>' . $row->reps . '</td>';
							print '<td>' . $row->movement . '</td>';
						print '</tr>';
					}
				print '</table>';			
			
			print '</div>';
		print '</div>';

		print '<div class="one_half last">';
	
			db_query('SELECT @last_id := MAX(wid) FROM {workout_builder_strength}');
			$result = db_query('SELECT * FROM {workout_builder_strength} WHERE wid = @last_id');
			print '<div id="wod">';
				print '<table>';
				
					print '<tr>';
						print '<th colspan="4"><a href="?q=workout_tracker&wid=' . $wid . '">' . 'Strength Portion:' . '</a></th>';	
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