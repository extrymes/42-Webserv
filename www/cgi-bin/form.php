#!/usr/bin/php

<?php

function print_html($name, $message) {
	echo "<!DOCTYPE html><html><body>";
	echo "<h1>Simple Form</h1>";
	echo "<p>name: " . (htmlspecialchars($name) ?: "Unknown") . "</p>";
	echo "<p>message: " . (htmlspecialchars($message) ?: "No message") . "</p>";
	echo "</html></body>";
}

// Get body in environment
$body = getenv("body");
if (!$body) {
	// Print HTML
	print_html(NULL, NULL);
	exit();
}

// Parse body into dictionary
$data_dict = [];
$splitted_body = explode("&", $body);
foreach ($splitted_body as $item) {
	$pair = explode("=", $item, 2);
	if (count($pair) === 2) {
		list($key, $value) = $pair;
		$data_dict[urldecode($key)] = urldecode($value);
	}
}

// Get each data from dictionary
$name = $data_dict["name"];
$message = $data_dict["message"];

// Print HTML
print_html($name, $message)

?>
