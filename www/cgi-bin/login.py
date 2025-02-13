#!/usr/bin/python3

# Import OS module
import os

# Get body in environment
body = os.environ.get("body")

while True:
	print("bonjour")

if not body:
	# std::string httpResponse = "HTTP/1.1 " + code + "\r\n";
	# httpResponse += "Content-Type: " + ext + "\r\n";
	# httpResponse += "Content-Length: " + toString(file.length()) + "\r\n";

	html = """
	<!DOCTYPE html><html><body>
	<h1>Hello Guest!</h1>
	</html></body>"""
	print("HTTP/1.1 200 OK")
	print("Content-Type: text/html")
	print("Content-Length: ", len(html))
	print("Connection: close\r\n\r\n")
	print(html)
	exit()

# Parse body into dictionary
data_dict = {}
splitted_body = body.split('&')
for item in splitted_body:
	pair = item.split('=', 1)
	if len(pair) == 2:
		key, value = pair
		data_dict[key] = value

# Get each data from dictionary
first_name = data_dict.get("first_name")
if not first_name or first_name == "":
	first_name = "Guest"
else:
	first_name = first_name.replace("+", " ")
last_name = data_dict.get("last_name")
if not last_name or last_name == "":
	last_name = "Unknown"
else:
	last_name = last_name.replace("+", " ")
email = data_dict.get("email")
if not email or last_name == "":
	email = "Unknown"
else:
	email = email.replace("%40", "@")
phone = data_dict.get("phone")
if not phone or last_name == "":
	phone = "Unknown"
password = data_dict.get("password")
if not password or password == "":
	password = "None"
else:
	password = '*' * len(password)

# Print HTML
html = f"""
	<!DOCTYPE html><html><body>
	<h1>Hello {first_name}!</h1>
	<p>First name: {first_name}</p>
	<p>Last name: {last_name}</p>
	<p>Email: {email}</p>
	<p>Phone: {phone}</p>
	<p>Password: {password}</p>
	</html></body>"""

print("HTTP/1.1 200 OK")
print("Content-Type: text/html")
print("Content-Length: ", len(html))
print("Connection: close\r\n\r\n")
print(html)
