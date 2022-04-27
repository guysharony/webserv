<html>
	<head>
		<title>Show variables</title>
	</head>
	<body>
	<?php
		echo '<p>Current Working Directory</p>';
		echo getcwd();

		echo '<p>Environment Variables</p>';

		foreach (getenv() as $key => $env) {
			echo "$key = $env<br>\n";
		};


		echo '<br><br><p>Server Variables</p>';

		foreach ($_SERVER as $key => $value) {
			echo "$key = $value<br>\n";
		};

		echo '<p>Done</p>';
	?>
 </body>
</html>
