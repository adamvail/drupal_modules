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
	drupal_add_css(path_to_theme() . '/max_reps.css');
	 //print_r($max_reps_form, TRUE);
	// wrap the form in some html markup in order to style it
	// with a .css file.
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