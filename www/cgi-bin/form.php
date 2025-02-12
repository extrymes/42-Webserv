#!/usr/bin/php

<?php

function print_html($name, $message) {
	$html = "<!DOCTYPE html><html><body>";
	$html .= "<h1>Simple Form</h1>";
	$html .= "<p>name: " . (htmlspecialchars($name) ?: "Unknown") . "</p>";
	$html .= "<p>message: " . (htmlspecialchars($message) ?: "No message") . "</p>";
	$html .= "</html></body>";

	echo "HTTP/1.1 200 OK\r\n";
	echo "Content-Type: text/html\r\n";
	echo "Content-Length: " . strlen($html) . "\r\n";
	echo "Connection: close\r\n\r\n";
	echo $html;
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
