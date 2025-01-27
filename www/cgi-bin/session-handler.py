#!/usr/bin/pyhon3

import os
import cgi
from session import Session

# Init session
session = Session()
session.load_session()

# Get form data
form = cgi.FieldStorage()
username = form.getvalue("username")

if username:
	session.set("username", username)

# Print headers
print("Content-Type: text/html")
if "SESSION_ID" not in os.environ.get("HTTP_COOKIE", ""):
	print(session.get_cookie_header())
print("")

# Print HTML
print("<html><body>")
print("<h1>Session Management with CGI</h1>")

if session.get("username"):
	print(f"<p>Welcome, {session.get("username")}!</p>")
else:
	print("<p>Please enter your username:></p>")
	print("<form method=\"POST\">")
	print("<input type=\"text\" name=\"username\">")
	print("<button type=\"submit\" value=\"Submit\">")
	print("</form>")

print("</body></html>")
