<html>
	<head>
		<title>Show variables</title>
	</head>
	<body>
	<?php
		$stderr = fopen('php://stderr', 'w');
		fwrite($stderr, PHP_EOL . "Running env.php" . PHP_EOL);
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
