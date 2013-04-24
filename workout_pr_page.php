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
	<?php
	
		//check include statements
		if(!@include '/var/www/sites/all/modules/workout_pr/workout_pr.inc') {
			include 'C:/wamp/www/drupal/sites/all/modules/workout_pr/workout_pr.inc';
			render_page();
		}
		else {
			render_page();
		}
		function render_page() {
			//render the workout results form to the page
			print '<div id="form">';
				print drupal_render(drupal_get_form('workout_pr')); 
			print '</div>'; //end form div
		}
	?>
</body>

</html>
