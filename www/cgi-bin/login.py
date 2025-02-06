#!/usr/bin/python3

# Import OS module
import os

# Get body in environment
body = os.environ.get("body")
if not body:
	print("<!DOCTYPE html><html><body>")
	print("<h1>Hello Guest!</h1>")
	print("</html></body>")
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
first_name = data_dict.get("first_name", "Unknown")
if first_name:
	first_name = first_name.replace("+", " ")
last_name = data_dict.get("last_name", "Unknown")
if last_name:
	last_name = last_name.replace("+", " ")
email = data_dict.get("email", "Unknown")
if email:
	email = email.replace("%40", "@")
phone = data_dict.get("phone", "Unknown")
password = data_dict.get("password", None)
if password:
	password = '*' * len(password)

# Print HTML
print("<!DOCTYPE html><html><body>")
print(f"<h1>Hello {first_name}!</h1>")
print(f"<p>First name: {first_name}</p>")
print(f"<p>Last name: {last_name}</p>")
print(f"<p>Email: {email}</p>")
print(f"<p>Phone: {phone}</p>")
print(f"<p>Password: {password}</p>")
print("</html></body>")
