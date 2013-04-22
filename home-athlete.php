<!DOCTYPE html>
<html>
<head>

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
	padding:20px;
	margin:5px;
}
#wod {
	float:right;
	margin:5px;
	width:60%;
}
#hist {
	clear:both;
	float:left;
	width:40%;
	margin:5px;
}
#graph {
	float:right;
	margin:5px;
	padding:20px;
	width:50%;
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
td{
	text-align:center;
}
.left-cell {
	text-align: left;
}
.centered-cell {
  text-align: center;
} 
</style>




<?php

	if(detect_mobile() == TRUE){
		header("Location: " . base_path() . "?q=workout_tracker");
		die();
	}
	
	//check include statements
	if(!@include '/var/www/sites/all/modules/workout_results/workout_results.inc') {
		include 'C:/wamp/www/drupal/sites/all/modules/workout_results/workout_results.inc';
		render_page();
	}
	else {
		render_page();
	}

	function render_page() {
		global $user;

		if ( $user->uid ) {
			print '<div id="prof">';
				
				$mygym = '';
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
					$weight_lb = db_query('SELECT SUM(work) FROM {workout_tracker_strength} WHERE athlete_uid = :myuid AND weight_units=:units', array(':myuid'=> $usr->uid, ':units' => 0))->fetchField();
					$weight_kg = db_query('SELECT SUM(work) FROM {workout_tracker_strength} WHERE athlete_uid = :myuid AND weight_units=:units', array(':myuid'=> $usr->uid, ':units' => 1))->fetchField();
					
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
					$mygym = $usr->gym;
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
			
			
			
			//logic to get the most recent WOD which can be either from the coach or the athlete
			$wid = 0;	
			// grab the most up to date wid the athlete has built
			$swida = db_query('SELECT MAX(wid) FROM {workout_builder_strength} WHERE creator_id = :auid and date < :date', array(':auid' => $user->uid, ':date' => time()))->fetchField();
			$cwida = db_query('SELECT MAX(wid) FROM {workout_builder_conditioning} WHERE creator_id = :auid and date < :date', array(':auid' => $user->uid, ':date' => time()))->fetchField();
			if($swida > $cwida) {
				$wid = $swida;
			}
			else {
				$wid = $cwida;
			}
			//grab the coaches uid
			$cuid = db_query('SELECT uid FROM {workout_gym_affiliation} WHERE gym LIKE :mygym AND role LIKE :role', array(':mygym' => $mygym, ':role' => 'coach'))->fetchField();
			// grab the most up to date wid the coach has built
			$swidc = db_query('SELECT MAX(wid) FROM {workout_builder_strength} WHERE creator_id = :cuid and date < :date', array(':cuid' => $cuid, ':date' => time()))->fetchField();
			$cwidc = db_query('SELECT MAX(wid) FROM {workout_builder_conditioning} WHERE creator_id = :cuid and date < :date', array(':cuid' => $cuid, ':date' => time()))->fetchField();
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
			
			$gwid = db_query('SELECT MAX(wid) FROM {workout_tracker_strength} WHERE athlete_uid = :myuid', array(':myuid' => $user->uid))->fetchField();
			if($gwid != 0){
				print '<div id="graph">';
					//*-------------------------------------------------
					
					//build their graph
					$chart = array(
						'#chart_id' => 'wod_chart',
						'#title' => chart_title(t('WOD Breakdown'), 15),
						'#type' => CHART_TYPE_BAR_V_GROUPED,
						'#size' => chart_size(400, 200),
						'#adjust_resolution' => TRUE, 
						'#bar_size' => chart_bar_size(15, 5), 
					);
					
					$workout = db_query('SELECT * FROM {workout_tracker_strength} WHERE wid = :wid AND athlete_uid = :auid', array(':wid' => $gwid, ':auid' => $user->uid));
					$exercise = db_query('SELECT movement FROM {workout_builder_strength} WHERE wid = :wid', array(':wid'=>$gwid));
					$size = db_query('SELECT sid FROM {workout_tracker_strength} WHERE wid = :wid AND athlete_uid = :auid ORDER BY sid DESC LIMIT 1', array(':wid' => $gwid, ':auid' => $user->uid))->fetchField();
					//initialize data array
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


					print theme('chart', array('chart' => $chart));

					//--------------------------------------------------*/
				print '</div>'; //end graph div
			}
		}
	}
	
	//helper print function
	function pretty_print($str){
		$pretty_str = implode(' ', array_map('ucfirst', explode(' ', $str)));
		return $pretty_str;
	}

function detect_mobile(){
	$useragent=$_SERVER['HTTP_USER_AGENT'];

	// do the mobile detection
	if (
			preg_match('/android.+mobile|avantgo|bada\/|blackberry|blazer|compal|elaine|fennec|hiptop|iemobile|ip(hone|od)|iris|kindle|lge |maemo|meego.+mobile|midp|mmp|netfront|opera m(ob|in)i|palm( os)?|phone|p(ixi|re)\/|plucker|pocket|psp|series(4|6)0|symbian|treo|up\.(browser|link)|vodafone|wap|windows (ce|phone)|xda|xiino/i',$useragent)
			||
			preg_match('/1207|6310|6590|3gso|4thp|50[1-6]i|770s|802s|a wa|abac|ac(er|oo|s\-)|ai(ko|rn)|al(av|ca|co)|amoi|an(ex|ny|yw)|aptu|ar(ch|go)|as(te|us)|attw|au(di|\-m|r |s )|avan|be(ck|ll|nq)|bi(lb|rd)|bl(ac|az)|br(e|v)w|bumb|bw\-(n|u)|c55\/|capi|ccwa|cdm\-|cell|chtm|cldc|cmd\-|co(mp|nd)|craw|da(it|ll|ng)|dbte|dc\-s|devi|dica|dmob|do(c|p)o|ds(12|\-d)|el(49|ai)|em(l2|ul)|er(ic|k0)|esl8|ez([4-7]0|os|wa|ze)|fetc|fly(\-|_)|g1 u|g560|gene|gf\-5|g\-mo|go(\.w|od)|gr(ad|un)|haie|hcit|hd\-(m|p|t)|hei\-|hi(pt|ta)|hp( i|ip)|hs\-c|ht(c(\-| |_|a|g|p|s|t)|tp)|hu(aw|tc)|i\-(20|go|ma)|i230|iac( |\-|\/)|ibro|idea|ig01|ikom|im1k|inno|ipaq|iris|ja(t|v)a|jbro|jemu|jigs|kddi|keji|kgt( |\/)|klon|kpt |kwc\-|kyo(c|k)|le(no|xi)|lg( g|\/(k|l|u)|50|54|\-[a-w])|libw|lynx|m1\-w|m3ga|m50\/|ma(te|ui|xo)|mc(01|21|ca)|m\-cr|me(di|rc|ri)|mi(o8|oa|ts)|mmef|mo(01|02|bi|de|do|t(\-| |o|v)|zz)|mt(50|p1|v )|mwbp|mywa|n10[0-2]|n20[2-3]|n30(0|2)|n50(0|2|5)|n7(0(0|1)|10)|ne((c|m)\-|on|tf|wf|wg|wt)|nok(6|i)|nzph|o2im|op(ti|wv)|oran|owg1|p800|pan(a|d|t)|pdxg|pg(13|\-([1-8]|c))|phil|pire|pl(ay|uc)|pn\-2|po(ck|rt|se)|prox|psio|pt\-g|qa\-a|qc(07|12|21|32|60|\-[2-7]|i\-)|qtek|r380|r600|raks|rim9|ro(ve|zo)|s55\/|sa(ge|ma|mm|ms|ny|va)|sc(01|h\-|oo|p\-)|sdk\/|se(c(\-|0|1)|47|mc|nd|ri)|sgh\-|shar|sie(\-|m)|sk\-0|sl(45|id)|sm(al|ar|b3|it|t5)|so(ft|ny)|sp(01|h\-|v\-|v )|sy(01|mb)|t2(18|50)|t6(00|10|18)|ta(gt|lk)|tcl\-|tdg\-|tel(i|m)|tim\-|t\-mo|to(pl|sh)|ts(70|m\-|m3|m5)|tx\-9|up(\.b|g1|si)|utst|v400|v750|veri|vi(rg|te)|vk(40|5[0-3]|\-v)|vm40|voda|vulc|vx(52|53|60|61|70|80|81|83|85|98)|w3c(\-| )|webc|whit|wi(g |nc|nw)|wmlb|wonu|x700|yas\-|your|zeto|zte\-/i',substr($useragent,0,4))
	)
		{
			return TRUE;
		}
	return FALSE;
}

?>

</body>
</html>
