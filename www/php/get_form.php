<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>GET Request form</title>
</head>
<body>
	<?php if (isset($_GET['first_name']) && isset($_GET['last_name'])) { ?>
		<div>
			<p>Received GET request</p>
			<p>First name: <?php echo $_GET['first_name']; ?></p>
			<p>Last name: <?php echo $_GET['last_name']; ?></p>
		</div>
	<?php } ?>
	<h1>GET Request</h1>
	<form action="/php/get_form.php" method="GET">
		<div>
			<label for="first_name">First name</label>
			<input type="text" name="first_name" id="first_name" value="">
		</div>
		<div>
			<label for="last_name">Last name</label>
			<input type="text" name="last_name" id="last_name" value="">
		</div>
		<input type="submit" value="Send">
	</form>
</body>
</html>