<html>
	<head>
		<title>PHP Test</title>
	</head>
	<body>
		<?php
			echo '<p>Hello World</p>';
		?> 
		<?php
			if (!empty($_SERVER['PATH_INFO'])) echo "Location: " . $_SERVER['PATH_INFO'];
		?>
	</body>
</html>