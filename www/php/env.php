<html>
	<head>
		<title>Show variables</title>
	</head>
	<body>
	<?php
		echo '<p>Environment Variables</p>';

		foreach (getenv() as $key => $env) {
			echo "$key = $env<br>\n";
		};
	?>
	<p><a href="../index.html">Home</a></p>
 </body>
</html>
