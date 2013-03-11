<?php
/**
 * @file
 *
 * This is the template file for rendering the formexample nameform.
 * In this file each element of the form is rendered individually
 * instead of the entire form at once, giving me the ultimate control
 * over how my forms are laid out. I could also print the whole form
 * at once - using the predefined layout in the module by
 * printing $variables['formexample_nameform_form'];
 *
 */

	print '<div id="max_reps">';
		print '<div id="sets">';
			print $max_reps['sets'];			
		print '</div>';
		print '<div id="x">';
			print ' X ';			
		print '</div>';
		print '<div id="reps">';
			print $max_reps['reps'];
		print '</div>';
		print '<div id="exercise">';
			print $max_reps['exercise'];
		print '</div>';
		print '<div id="submit">';
			print $max_reps['submit'];
		print '</div>';
	print '</div>';
?>

<style type="text/css">

	#sets{
		float:left;
		padding: 0px 10px 0px 20px;
	}
	#x{
		float:left;
		padding: 25px 0px 0px 0px;
	}
	#reps{
		float:left;
		padding: 0px 20px 0px 10px;
	}
	#exercise{
		float:left;
		padding: 0px 20px 0px 20px;
	}
	#submit{
		clear:both;
	}

	
	</style>