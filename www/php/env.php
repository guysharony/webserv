<html>
 <head>
  <title>Show env</title>
 </head>
 <body>
 <?php echo '<p>Environment Variables</p>'; ?> 
 <?php
 foreach (getenv() as $key => $env) {
	echo $key;
	echo " = ";
	echo $env;
	echo "<br>\n";
 };
?>

<?php echo "<br><br>\n\n<p>Server Variables</p>"; ?> 
 <?php
 foreach ($_SERVER as $key => $value) {
	echo $key;
	echo " = ";
	echo $value;
	echo "<br>\n";
 };
	// if (!empty($_SERVER['PATH_INFO'])) echo "Location: " . $_SERVER['PATH_INFO'];
?>

<?php echo '<p>Done</p>'; ?> 
 </body>
</html>
