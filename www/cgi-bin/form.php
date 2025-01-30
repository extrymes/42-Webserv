#!usr/bin/php

<?php

// Get form data
$name = isset($_GET["name"]) ? $_GET["name"] : "Guest";
$message = isset($_GET["message"]) ? $_GET["message"] : "No message";

// Define header
header("Content-Type: text/html");

// Print HTML
echo "<html><body>";
echo "<h1>Simple Form</h1>";
echo "<p>name: " . htmlspecialchars($name) . "</p>";
echo "<p>message: " . htmlspecialchars($message) . "</p>";
echo "</html></body>";

?>
