<!DOCTYPE html>
<html>
<body>

<style>
/* max_reps style sheet */

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
			$wid = db_query('SELECT MAX(wid) FROM {workout_builder_conditioning}')->fetchField();
			print '<div id="cond">';
				print '<table>';
					print '<tr>';
						print '<th colspan="0"><a href="?q=workout_tracker&wid=' . (int)$wid . '">' . 'Conditioning Portion:' . '</a></th>';	
					print '</tr>';
					
					$i = 0;
					foreach ($result as $row) {
						//create the header for each separate portion of the workout
						if($i != $row->conditioning_id){
							if($row->style == 'amrap'){
								if($row->wgt_choice == 'distance'){
									if($row->distance_units == 0){									
										if($row->distance_men != NULL && $row->distance_women != NULL){
											print '<tr>';
												print '<th>' . 'Style' . '</th>';
												print '<th>' . 'Duration' . '</th>';
												print '<th>' . 'Reps' . '</th>';
												print '<th>' . 'Movement' . '</th>';
												print '<th>' . 'Dist Men/Women (m)' . '</th>';
											print '</tr>';
										}
										elseif($row->distance_self_rx != NULL){
											print '<tr>';
												print '<th>' . 'Style' . '</th>';
												print '<th>' . 'Duration' . '</th>';
												print '<th>' . 'Reps' . '</th>';
												print '<th>' . 'Movement' . '</th>';
												print '<th>' . 'Distance (m)' . '</th>';
											print '</tr>';
										}
									
									}
									elseif($row->distance_units == 1){
										if($row->distance_men != NULL && $row->distance_women != NULL){
											print '<tr>';
												print '<th>' . 'Style' . '</th>';
												print '<th>' . 'Duration' . '</th>';
												print '<th>' . 'Reps' . '</th>';
												print '<th>' . 'Movement' . '</th>';
												print '<th>' . 'Dist Men/Women (in)' . '</th>';
											print '</tr>';
										}
										elseif($row->distance_self_rx != NULL){
											print '<tr>';
												print '<th>' . 'Style' . '</th>';
												print '<th>' . 'Duration' . '</th>';
												print '<th>' . 'Reps' . '</th>';
												print '<th>' . 'Movement' . '</th>';
												print '<th>' . 'Distance (in)' . '</th>';
											print '</tr>';
										}					
									}
									elseif($row->distance_units == 2){
										if($row->distance_men != NULL && $row->distance_women != NULL){
											print '<tr>';
												print '<th>' . 'Style' . '</th>';
												print '<th>' . 'Duration' . '</th>';
												print '<th>' . 'Reps' . '</th>';
												print '<th>' . 'Movement' . '</th>';
												print '<th>' . 'Dist Men/Women (cal)' . '</th>';
											print '</tr>';
										}
										elseif($row->distance_self_rx != NULL){
											print '<tr>';
												print '<th>' . 'Style' . '</th>';
												print '<th>' . 'Duration' . '</th>';
												print '<th>' . 'Reps' . '</th>';
												print '<th>' . 'Movement' . '</th>';
												print '<th>' . 'Distance (cal)' . '</th>';
											print '</tr>';
										}
									}						
								}
								elseif($row->wgt_choice == 'weight'){
									if($row->wgt_style == 'percentage'){
										print '<tr>';
											print '<th>' . 'Style' . '</th>';
											print '<th>' . 'Duration' . '</th>';
											print '<th>' . 'Reps' . '</th>';
											print '<th>' . 'Movement' . '</th>';
											print '<th>' . 'Weight (%)' . '</th>';
										print '</tr>';
									}
									elseif($row->wgt_style == 'weight'){
										if($row->wgt_men != NULL && $row->wgt_women != NULL){
											print '<tr>';
												print '<th>' . 'Style' . '</th>';
												print '<th>' . 'Duration' . '</th>';
												print '<th>' . 'Reps' . '</th>';
												print '<th>' . 'Movement' . '</th>';
												print '<th>' . 'Weight Men/Women (lbs)' . '</th>';
											print '</tr>';
										}
										elseif($row->wgt_self_rx != NULL){
											print '<tr>';
												print '<th>' . 'Style' . '</th>';
												print '<th>' . 'Duration' . '</th>';
												print '<th>' . 'Reps' . '</th>';
												print '<th>' . 'Movement' . '</th>';
												print '<th>' . 'Weight (lbs)' . '</th>';
											print '</tr>';
										}
									}		
								}
							}
							elseif($row->style == 'for_time'){
								if($row->wgt_choice == 'distance'){
									if($row->distance_units == 0){
										if($row->distance_men != NULL && $row->distance_women != NULL){
											print '<tr>';
												print '<th>' . 'Style' . '</th>';
												print '<th>' . 'Rounds/Cutoff (min)' . '</th>';
												print '<th>' . 'Reps' . '</th>';
												print '<th>' . 'Movement' . '</th>';
												print '<th>' . 'Dist Men/Women (m)' . '</th>';
											print '</tr>';
										}
										elseif($row->distance_self_rx != NULL){
											print '<tr>';
												print '<th>' . 'Style' . '</th>';
												print '<th>' . 'Rounds/Cutoff (min)' . '</th>';
												print '<th>' . 'Reps' . '</th>';
												print '<th>' . 'Movement' . '</th>';
												print '<th>' . 'Distance (m)' . '</th>';
											print '</tr>';
										}
									
									}
									elseif($row->distance_units == 1){
										if($row->distance_men != NULL && $row->distance_women != NULL){
											print '<tr>';
												print '<th>' . 'Style' . '</th>';
												print '<th>' . 'Rounds/Cutoff (min)' . '</th>';
												print '<th>' . 'Reps' . '</th>';
												print '<th>' . 'Movement' . '</th>';
												print '<th>' . 'Dist Men/Women (in)' . '</th>';
											print '</tr>';
										}
										elseif($row->distance_self_rx != NULL){
											print '<tr>';
												print '<th>' . 'Style' . '</th>';
												print '<th>' . 'Rounds/Cutoff (min)' . '</th>';
												print '<th>' . 'Reps' . '</th>';
												print '<th>' . 'Movement' . '</th>';
												print '<th>' . 'Distance (in)' . '</th>';
											print '</tr>';
										}					
									}
									elseif($row->distance_units == 2){
										if($row->distance_men != NULL && $row->distance_women != NULL){
											print '<tr>';
												print '<th>' . 'Style' . '</th>';
												print '<th>' . 'Rounds/Cutoff (min)' . '</th>';
												print '<th>' . 'Reps' . '</th>';
												print '<th>' . 'Movement' . '</th>';
												print '<th>' . 'Dist Men/Women (cal)' . '</th>';
											print '</tr>';
										}
										elseif($row->distance_self_rx != NULL){
											print '<tr>';
												print '<th>' . 'Style' . '</th>';
												print '<th>' . 'Rounds/Cutoff (min)' . '</th>';
												print '<th>' . 'Reps' . '</th>';
												print '<th>' . 'Movement' . '</th>';
												print '<th>' . 'Distance (cal)' . '</th>';
											print '</tr>';
										}
									}						
								}
								elseif($row->wgt_choice == 'weight'){
									if($row->wgt_style == 'percentage'){
										print '<tr>';
											print '<th>' . 'Style' . '</th>';
											print '<th>' . 'Rounds/Cutoff (min)' . '</th>';
											print '<th>' . 'Reps' . '</th>';
											print '<th>' . 'Movement' . '</th>';
											print '<th>' . 'Weight (%)' . '</th>';
										print '</tr>';
									}
									elseif($row->wgt_style == 'weight'){
										if($row->wgt_men != NULL && $row->wgt_women != NULL){
											print '<tr>';
												print '<th>' . 'Style' . '</th>';
												print '<th>' . 'Rounds/Cutoff (min)' . '</th>';
												print '<th>' . 'Reps' . '</th>';
												print '<th>' . 'Movement' . '</th>';
												print '<th>' . 'Weight Men/Women (lbs)' . '</th>';
											print '</tr>';
										}
										elseif($row->wgt_self_rx != NULL){
											print '<tr>';
												print '<th>' . 'Style' . '</th>';
												print '<th>' . 'Rounds/Cutoff (min)' . '</th>';
												print '<th>' . 'Reps' . '</th>';
												print '<th>' . 'Movement' . '</th>';
												print '<th>' . 'Weight (lbs)' . '</th>';
											print '</tr>';
										}
									}		
								}
							}
							elseif($row->style == 'intervals'){
								if($row->wgt_choice == 'distance'){
									if($row->distance_units == 0){
										if($row->distance_men != NULL && $row->distance_women != NULL){
											print '<tr>';
												print '<th>' . 'Style' . '</th>';
												print '<th>' . 'Rounds' . '</th>';
												print '<th>' . 'Duration (sec)' . '</th>';
												print '<th>' . 'Movement' . '</th>';
												print '<th>' . 'Dist Men/Women (m)' . '</th>';
											print '</tr>';
										}
										elseif($row->distance_self_rx != NULL){
											print '<tr>';
												print '<th>' . 'Style' . '</th>';
												print '<th>' . 'Rounds' . '</th>';
												print '<th>' . 'Duration (sec)' . '</th>';
												print '<th>' . 'Movement' . '</th>';
												print '<th>' . 'Distance (m)' . '</th>';
											print '</tr>';
										}
									
									}
									elseif($row->distance_units == 1){
										if($row->distance_men != NULL && $row->distance_women != NULL){
											print '<tr>';
												print '<th>' . 'Style' . '</th>';
												print '<th>' . 'Rounds' . '</th>';
												print '<th>' . 'Duration (sec)' . '</th>';
												print '<th>' . 'Movement' . '</th>';
												print '<th>' . 'Dist Men/Women (in)' . '</th>';
											print '</tr>';
										}
										elseif($row->distance_self_rx != NULL){
											print '<tr>';
												print '<th>' . 'Style' . '</th>';
												print '<th>' . 'Rounds' . '</th>';
												print '<th>' . 'Duration (sec)' . '</th>';
												print '<th>' . 'Movement' . '</th>';
												print '<th>' . 'Distance (in)' . '</th>';
											print '</tr>';
										}					
									}
									elseif($row->distance_units == 2){
										if($row->distance_men != NULL && $row->distance_women != NULL){
											print '<tr>';
												print '<th>' . 'Style' . '</th>';
												print '<th>' . 'Rounds' . '</th>';
												print '<th>' . 'Duration (sec)' . '</th>';
												print '<th>' . 'Movement' . '</th>';
												print '<th>' . 'Dist Men/Women (cal)' . '</th>';
											print '</tr>';
										}
										elseif($row->distance_self_rx != NULL){
											print '<tr>';
												print '<th>' . 'Style' . '</th>';
												print '<th>' . 'Rounds' . '</th>';
												print '<th>' . 'Duration (sec)' . '</th>';
												print '<th>' . 'Movement' . '</th>';
												print '<th>' . 'Distance (cal)' . '</th>';
											print '</tr>';
										}
									}						
								}
								elseif($row->wgt_choice == 'weight'){
									if($row->wgt_style == 'percentage'){
										print '<tr>';
											print '<th>' . 'Style' . '</th>';
											print '<th>' . 'Rounds' . '</th>';
											print '<th>' . 'Duration (sec)' . '</th>';
											print '<th>' . 'Movement' . '</th>';
											print '<th>' . 'Weight (%)' . '</th>';
										print '</tr>';
									}
									elseif($row->wgt_style == 'weight'){
										if($row->wgt_men != NULL && $row->wgt_women != NULL){
											print '<tr>';
												print '<th>' . 'Style' . '</th>';
												print '<th>' . 'Rounds' . '</th>';
												print '<th>' . 'Duration (sec)' . '</th>';
												print '<th>' . 'Movement' . '</th>';
												print '<th>' . 'Weight Men/Women (lbs)' . '</th>';
											print '</tr>';
										}
										elseif($row->wgt_self_rx != NULL){
											print '<tr>';
												print '<th>' . 'Style' . '</th>';
												print '<th>' . 'Rounds' . '</th>';
												print '<th>' . 'Duration (sec)' . '</th>';
												print '<th>' . 'Movement' . '</th>';
												print '<th>' . 'Weight (lbs)' . '</th>';
											print '</tr>';
										}
									}		
								}
							}
							$i = $row->conditioning_id;
						}
						//write values of the workout
						if($row->style == 'amrap'){
							if($row->wgt_choice == 'distance'){						
								if($row->distance_men != NULL && $row->distance_women != NULL){
									print '<tr>' ;
										print '<td>' . $row->style . '</td>';
										print '<td>' . $row->duration . '</td>';
										print '<td>' . $row->reps . '</td>';
										print '<td>' . $row->movement . '</td>';
										print '<td>' . $row->distance_men . '/' . $row->distance_women . '</td>';
									print '</tr>';
								}
								elseif($row->distance_self_rx != NULL){
									print '<tr>' ;
										print '<td>' . $row->style . '</td>';
										print '<td>' . $row->duration . '</td>';
										print '<td>' . $row->reps . '</td>';
										print '<td>' . $row->movement . '</td>';
										print '<td>' . $row->distance_self_rx . '</td>';
									print '</tr>';
								}								
							}
							elseif($row->wgt_choice == 'weight'){
								if($row->wgt_style == 'percentage'){
									print '<tr>' ;
											print '<td>' . $row->style . '</td>';
											print '<td>' . $row->duration . '</td>';
											print '<td>' . $row->reps . '</td>';
											print '<td>' . $row->movement . '</td>';
											print '<td>' . $row->wgt_percentage . '</td>';
										print '</tr>';
								}
								elseif($row->wgt_style == 'weight'){
									if($row->wgt_men != NULL && $row->wgt_women != NULL){
										print '<tr>' ;
											print '<td>' . $row->style . '</td>';
											print '<td>' . $row->duration . '</td>';
											print '<td>' . $row->reps . '</td>';
											print '<td>' . $row->movement . '</td>';
											print '<td>' . $row->wgt_men . '/' . $row->wgt_women . '</td>';
										print '</tr>';
									}
									elseif($row->wgt_self_rx != NULL){
										print '<tr>' ;
											print '<td>' . $row->style . '</td>';
											print '<td>' . $row->duration . '</td>';
											print '<td>' . $row->reps . '</td>';
											print '<td>' . $row->movement . '</td>';
											print '<td>' . $row->wgt_self_rx . '</td>';
										print '</tr>';
									}
								}		
							}
						}
						elseif($row->style == 'for_time'){
							if($row->wgt_choice == 'distance'){			
								if($row->distance_men != NULL && $row->distance_women != NULL){
									print '<tr>' ;
										print '<td>' . $row->style . '</td>';
										print '<td>' . $row->rounds . '/' . $rows->cutoff . '</td>';
										print '<td>' . $row->reps . '</td>';
										print '<td>' . $row->movement . '</td>';
										print '<td>' . $row->distance_men . '/' . $row->distance_women . '</td>';
									print '</tr>';
								}
								elseif($row->distance_self_rx != NULL){
									print '<tr>' ;
										print '<td>' . $row->style . '</td>';
										print '<td>' . $row->rounds . '/' . $rows->cutoff . '</td>';
										print '<td>' . $row->reps . '</td>';
										print '<td>' . $row->movement . '</td>';
										print '<td>' . $row->distance_self_rx . '</td>';
									print '</tr>';
								}						
							}
							elseif($row->wgt_choice == 'weight'){
								if($row->wgt_style == 'percentage'){
									print '<tr>' ;
										print '<td>' . $row->style . '</td>';
										print '<td>' . $row->rounds . '/' . $row->cutoff . '</td>';
										print '<td>' . $row->reps . '</td>';
										print '<td>' . $row->movement . '</td>';
										print '<td>' . $row->wgt_percentage . '</td>';
									print '</tr>';
								}
								elseif($row->wgt_style == 'weight'){
									if($row->wgt_men != NULL && $row->wgt_women != NULL){
										print '<tr>' ;
											print '<td>' . $row->style . '</td>';
											print '<td>' . $row->rounds . '/' . $row->cutoff . '</td>';
											print '<td>' . $row->reps . '</td>';
											print '<td>' . $row->movement . '</td>';
											print '<td>' . $row->wgt_men . '/' . $row->wgt_women . '</td>';
										print '</tr>';
									}
									elseif($row->wgt_self_rx != NULL){
										print '<tr>' ;
											print '<td>' . $row->style . '</td>';
											print '<td>' . $row->rounds . '/' . $rows->cutoff . '</td>';
											print '<td>' . $row->reps . '</td>';
											print '<td>' . $row->movement . '</td>';
											print '<td>' . $row->wgt_self_rx . '</td>';
										print '</tr>';
									}
								}		
							}
						}
						elseif($row->style == 'intervals'){
							if($row->wgt_choice == 'distance'){	
								if($row->distance_men != NULL && $row->distance_women != NULL){
									print '<tr>' ;
										print '<td>' . $row->style . '</td>';
										print '<td>' . $row->rounds. '</td>';
										print '<td>' . $row->interval_time . '</td>';
										print '<td>' . $row->movement . '</td>';
										print '<td>' . $row->distance_men . '/' . $row->distance_women . '</td>';
									print '</tr>';
								}
								elseif($row->distance_self_rx != NULL){
									print '<tr>' ;
										print '<td>' . $row->style . '</td>';
										print '<td>' . $row->rounds. '</td>';
										print '<td>' . $row->interval_time . '</td>';
										print '<td>' . $row->movement . '</td>';
										print '<td>' . $row->distance_self_rx . '</td>';
									print '</tr>';
								}											
							}
							elseif($row->wgt_choice == 'weight'){
								if($row->wgt_style == 'percentage'){
									print '<tr>' ;
										print '<td>' . $row->style . '</td>';
										print '<td>' . $row->rounds. '</td>';
										print '<td>' . $row->interval_time . '</td>';
										print '<td>' . $row->movement . '</td>';
										print '<td>' . $row->wgt_percentage . '</td>';
									print '</tr>';
								}
								elseif($row->wgt_style == 'weight'){
									if($row->wgt_men != NULL && $row->wgt_women != NULL){
										print '<tr>' ;
											print '<td>' . $row->style . '</td>';
											print '<td>' . $row->rounds. '</td>';
											print '<td>' . $row->interval_time . '</td>';
											print '<td>' . $row->movement . '</td>';
											print '<td>' . $row->wgt_men . '/' . $row->wgt_women . '</td>';
										print '</tr>';
									}
									elseif($row->wgt_self_rx != NULL){
										print '<tr>' ;
											print '<td>' . $row->style . '</td>';
											print '<td>' . $row->rounds. '</td>';
											print '<td>' . $row->interval_time . '</td>';
											print '<td>' . $row->movement . '</td>';
											print '<td>' . $row->wgt_self_rx . '</td>';
										print '</tr>';
									}
								}		
							}
						}							
					}
				print '</table>';					
		print '</div>';

		print '<div class="one_half last">';
			
	
			db_query('SELECT @last_id := MAX(wid) FROM {workout_builder_strength}');
			$result = db_query('SELECT * FROM {workout_builder_strength} WHERE wid = @last_id');
			$wid = db_query('SELECT MAX(wid) FROM {workout_builder_strength}')->fetchField();
			print '<div id="wod">';
				print '<table>';
				
					print '<tr>';
						print '<th colspan="0"><a href="?q=workout_tracker&wid=' . (int)$wid . '">' . 'Strength Portion:' . '</a></th>';	
					print '</tr>';
					
					$i = 0;
					foreach ($result as $row) {
						// write the table header per strength_id
						if($i != $row->strength_id){
							// weight as percentage header
							if($row->wgt_style == 'percentage'){
								print '<tr>' ;
									print '<th>' . 'Clock (min)' . '</th>';
									print '<th>' . 'Sets' . '</th>';
									print '<th>' . 'Reps' . '</th>';
									print '<th>' . 'Movement' . '</th>';
									print '<th>' . 'Weight (%)' . '</th>';
								print '</tr>';
							}
							// weight as specific header
							elseif($row->wgt_style == 'weight'){ 
								// weight as men and women header
								if($row->wgt_men != NULL && $row->wgt_women != NULL){
									print '<tr>' ;
										print '<th>' . 'Clock (min)' . '</th>';
										print '<th>' . 'Sets' . '</th>';
										print '<th>' . 'Reps' . '</th>';
										print '<th>' . 'Movement' . '</th>';
										print '<th>' . 'Weight Men/Women (lbs)' . '</th>';
									print '</tr>';
								}
								// weight as self created header
								elseif($row->wgt_self_rx != NULL){
									print '<tr>' ;
										print '<th>' . 'Clock (min)' . '</th>';
										print '<th>' . 'Sets' . '</th>';
										print '<th>' . 'Reps' . '</th>';
										print '<th>' . 'Movement' . '</th>';
										print '<th>' . 'Weight (lbs)' . '</th>';
									print '</tr>';															
								}
							}
							elseif($row->wgt_style == 'ahap'){
								print '<tr>' ;
									print '<th>' . 'Clock (min)' . '</th>';
									print '<th>' . 'Sets' . '</th>';
									print '<th>' . 'Reps' . '</th>';
									print '<th>' . 'Movement' . '</th>';
									print '<th>' . 'Weight (AHAP)' . '</th>';
								print '</tr>';							
							}
							$i = $row->strength_id;
						}
						// write the values of the workout
						if($row->wgt_style == 'percentage'){						
							print '<tr>' ;
								print '<td>' . $row->clock . '</td>';
								print '<td>' . $row->sets . '</td>';
								print '<td>' . $row->reps . '</td>';
								print '<td>' . $row->movement . '</td>';
								print '<td>' . $row->wgt_percentage . '</td>';
							print '</tr>';
						}
						elseif($row->wgt_style == 'weight'){
							if($row->wgt_men != NULL && $row->wgt_women != NULL){						
								print '<tr>' ;
									print '<td>' . $row->clock . '</td>';
									print '<td>' . $row->sets . '</td>';
									print '<td>' . $row->reps . '</td>';
									print '<td>' . $row->movement . '</td>';
									print '<td>' . $row->wgt_men . '/' . $row->wgt_women . '</td>';
								print '</tr>';
							}
							elseif($row->wgt_self_rx != NULL){
								print '<tr>' ;
									print '<td>' . $row->clock . '</td>';
									print '<td>' . $row->sets . '</td>';
									print '<td>' . $row->reps . '</td>';
									print '<td>' . $row->movement . '</td>';
									print '<td>' . $row->wgt_self_rx . '</td>';
								print '</tr>';				
							}		
						}
						elseif($row->wgt_style == 'ahap'){
							print '<tr>' ;
								print '<td>' . $row->clock . '</td>';
								print '<td>' . $row->sets . '</td>';
								print '<td>' . $row->reps . '</td>';
								print '<td>' . $row->movement . '</td>';
								print '<td>' . 'As Heavy As Possible' . '</td>';
							print '</tr>';	
						}			
					}
				print '</table>';
			print '</div>';
		print '</div>';
	}
?>

</body>
</html>