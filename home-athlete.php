<!DOCTYPE html>
<html>
<body>

<style>
/* max_reps style sheet */

#title{
	text-align:center;
}
td{
	text-align:center;
}
</style>




<?php
	include 'C:\wamp\www\drupal\sites\all\modules\workout_tracker\utils.inc';
	global $user;

	if ( $user->uid ) {
		print '<div id="wod">';
		$result = db_query('SELECT gym FROM {workout_gym_affiliation} WHERE uid = :uid', array(':uid' => $user->uid));
		foreach($result as $name){
			db_query('SELECT @last_id := MAX(wid) FROM {workout_builder_conditioning}');
			$ret = db_query('SELECT * FROM {workout_builder_strength} WHERE wid = @last_id AND gym LIKE :gym', array(':gym' => $name->gym));
			foreach($ret as $row){
				$output = build_strength_text($row->wid);
				print '<table>';
					print '<tr>';
						print '<th colspan="0"><a  href="?q=workout_tracker&wid=' . (int)$row->wid . '">' . 'Workout of the Day' . '</a></th>';	
					print '</tr>';
					print '<tr>';
						print '<th colspan="0">' . 'Strength Portion:' . '</th>';	
					print '</tr>';
					print '<tr>';
						print '<td>' . $output . '</td>';
					print '</tr>';
					print '<tr>';
						print '<th colspan="0">' . 'Conditioning Portion:' . '</th>';	
					print '</tr>';
					print '<tr>';
						print '<td>' . $output . '</td>';
					print '</tr>';
				print '</table>';
			}
		}
		print '</div>';
		print '<div>';
			
		print '</div>';
	}
?>

</body>
</html>