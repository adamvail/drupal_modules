<!DOCTYPE html>
<html>
<head>

<script src="//ajax.googleapis.com/ajax/libs/jquery/1.9.1/jquery.min.js"></script>

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
				if(e.style.display == 'block') {
					e.style.display = 'none';
				}
				else {
					document.getElementByClassName("hidden-prof").style.display = 'none';
					e.style.display = 'block';
				}
			}
		}
		xmlhttp.open("GET","http://localhost/drupal/?q=members",true);
		xmlhttp.send();
    }

</script>

<script>
	function hideallexcept(className, must_show) {
		var els = document.getElementsByTagName('div')
		for (i=0;i<els.length; i++) {
			if (els.item(i).className == className){
				els.item(i).style.display="none";
			}
		}
		document.getElementById(must_show).style.display='block';
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
/* members style sheet */

#members {
	width:50%;
	margin:10px;
	float:left;
}
#prof {
	border-style:solid;
	border-width:2px;
	border-color:#D4D4D4;
	border-radius:10px;
	float:right;
	padding:20px;
	margin:5px;
	width:70%;
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
.centered-cell {
  text-align:center;
}
.hidden-prof {
	display:none;
	float:right;
	width:45%;
}
.hidden-results {
	display:none;
	clear:both;
	padding:40px;
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
		
		function render_page() {
			
			global $user;
			$cindex = 0;
		
			print '<div id="members">';
			
				print '<table>';
				
					$gym = db_query('SELECT gym FROM {workout_gym_affiliation} WHERE uid =:uid', array(':uid' => $user->uid))->fetchField();
					$result = db_query('SELECT * FROM {workout_gym_affiliation} WHERE gym LIKE :mygym', array(':mygym' => $gym));
					
					print '<tr><th colspan="0" class="centered-cell">' . pretty_print($gym) . ' Members </th></tr>'; 
					
					$members;
					foreach($result as $member){
						// display their name and role in the member list
						print '<tr><td><a href="/drupal/#" onclick="hideallexcept(\'hidden-prof\',\'memberprof' . $cindex . '\'); hideallexcept(\'hidden-results\', \'memberresults' . $cindex . '\'); return false;">' . pretty_print($member->name) .'</a></td><td>' . pretty_print($member->role) . '</td></tr>';
						$members[$cindex] = $member->uid;
						$cindex++;
						
					}
				
				print '</table>';
			print '</div>'; //end members div
			
			//display a members profile\
			for($i=0; $i<$cindex; $i++){
				print '<div class="hidden-prof" id="memberprof' . $i . '">';
					print '<div id="prof">';
				
						//$mygym = '';
						$result = db_query('SELECT * FROM {workout_gym_affiliation} WHERE uid = :uid', array(':uid' => $members[$i]));
						foreach($result as $usr){
							//name and gym affiliation section
							print '<div id="uname">';
								print '<p id="uname"><font color="179ce8" size="5"> ' . pretty_print($usr->name) . '</font>';
								print '<br>';
								print '<em>' . pretty_print($usr->gym) . '</em></p>';
								print '<hr>';
							print '</div>'; //end uname div
							
							$twods = db_query('SELECT COUNT(DISTINCT wid) FROM {workout_tracker_strength} WHERE athlete_uid = :myuid', array(':myuid'=> $members[$i]))->fetchField();
							$weight_lb = db_query('SELECT SUM(work) FROM {workout_tracker_strength} WHERE athlete_uid = :myuid AND weight_units=:units', array(':myuid'=> $members[$i], ':units' => 0))->fetchField();
							$weight_kg = db_query('SELECT SUM(work) FROM {workout_tracker_strength} WHERE athlete_uid = :myuid AND weight_units=:units', array(':myuid'=> $members[$i], ':units' => 1))->fetchField();
							
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
							//$mygym = $usr->gym;
						}
						
						print '<div id="prpic">';
							$picture_uri = db_query("SELECT uri FROM {file_managed} where uid=:uid", array(':uid' => $members[$i]))->fetchField();
							if($picture_uri != NULL){
								print '<img src="' . file_create_url($picture_uri) . '">';			
							}
							else {		
								print '<img src="' . base_path() . 'misc/druplicon.png">';
							}
						print '</div>'; //end prpic div
						
					print '</div>'; //end prof div
				print '</div>';
			}
			
			//display a members last completed workout\
			for($i=0; $i<$cindex; $i++){
				print '<div class="hidden-results" id="memberresults' . $i . '">';
				
					//logic to get the most recent WOD which can be either from the coach or the athlete
					$wid = 0;	
					// grab the most up to date wid the athlete has built
					$swida = db_query('SELECT MAX(wid) FROM {workout_tracker_strength} WHERE athlete_uid = :auid and date < :date', array(':auid' => $members[$i], ':date' => time()))->fetchField();
					$cwida = db_query('SELECT MAX(wid) FROM {workout_tracker_conditioning} WHERE athlete_uid = :auid and date < :date', array(':auid' => $members[$i], ':date' => time()))->fetchField();
					if($swida > $cwida) {
						$wid = $swida;
					}
					else {
						$wid = $cwida;
					}
					
					$strength_str = create_strength_string($wid, 1);
					$conditioning_str = create_conditioning_string($wid, 1);
					
					if(!empty($strength_str) || !empty($conditioning_str)) {
						print '<table>';
							
							print '<tr><th colspan="0" class="centered-cell"> Most Recent WOD </th></tr>';
							if(!empty($strength_str)) {
								print '<tr><th colspan="0"> Strength Portion </th></tr>';
								print '<tr><td>' . $strength_str . '<td><tr>';
							}
							if(!empty($conditioning_str)) {
								print '<tr><th colspan="0"> Conditioning Portion </th></tr>';
								print '<tr><td>' . $strength_str . '<td><tr>';
							}
							
						print '</table>';
					}
				print '</div>';
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
