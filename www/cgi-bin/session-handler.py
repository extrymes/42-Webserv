#!/usr/bin/python3

import os
from session import Session

# Init session
session = Session()
session.load_session()

# Get body
body = os.environ.get("body", None)

if body:
	# Parse body into dictionary
	data_dict = {}
	splitted_body = body.split('&')
	for item in splitted_body:
		pair = item.split('=', 1)
		if len(pair) == 2:
			key, value = pair
			data_dict[key] = value

	username = data_dict["username"]
	if username:
		session.set("username", username)

# Construct HTML
html = """
	<!DOCTYPE html><html><body>
	<h1>Session Management with CGI</h1>"""
if session.get("username"):
	html += f"<p>Welcome, {session.get('username')}!</p>"
else:
	html += """
	<p>Please enter your username:</p>
	<form method=\"POST\">
	<input type=\"text\" name=\"username\">
	<button type=\"text\" placeholder=\"Your username\">SUBMIT</button>
	</form>"""
html += "</body></html>"

# Print headers
print("HTTP/1.1 200 OK")
print("Content-Type:", "text/html")
print("Content-Length:", len(html))
print("Set-Cookie:", session.get_cookie_header())
print("Connection: close\r\n\r\n")

# Print HTML
print(html)
