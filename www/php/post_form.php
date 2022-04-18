<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>POST Request form</title>
</head>
<body>
	<?php if (isset($_POST['first_name']) && isset($_POST['last_name'])) { ?>
		<div>
			<p>Received POST request</p>
			<p>First name: <?php echo $_POST['first_name']; ?></p>
			<p>Last name: <?php echo $_POST['last_name']; ?></p>
		</div>
	<?php } ?>
	<h1>POST Request</h1>
	<form action="/php/post_form.php" method="POST">
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