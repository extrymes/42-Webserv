#!/usr/bin/php

<?php

// Get form data
$body = getenv("body");
if (!$body) {
	echo "<html><body>";
	echo "<h1>Simple Form</h1>";
	echo "<p>name: Unknown</p>";
	echo "<p>message: No message</p>";
	echo "</html></body>";
	exit();
}

# Parse body into dictionary
$data_dict = [];
$splitted_body = explode("&", $body);
foreach ($splitted_body as $item) {
	$pair = explode("=", $item, 2);
	if (count($pair) === 2) {
		list($key, $value) = $pair;
		$data_dict[urldecode($key)] = urldecode($value);
	}
}

// Get each data
$name = $data_dict["name"];
$message = $data_dict["message"];

// Print HTML
echo "<html><body>";
echo "<h1>Simple Form</h1>";
echo "<p>name: " . htmlspecialchars($name) . "</p>";
echo "<p>message: " . htmlspecialchars($message) . "</p>";
echo "</html></body>";

?>
