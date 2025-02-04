#!/usr/bin/python3

# Import OS module
import os

# Get body in environment
body = os.environ.get("body")
if not body:
	exit()

# Parse body into dictionary
data_dict = {}
splitted_body = body.split('&')
for item in splitted_body:
	pair = item.split('=', 1)
	if len(pair) == 2:
		key, value = pair
		data_dict[key] = value

# Get each data
first_name = data_dict["first_name"].replace("+", " ")
last_name = data_dict["last_name"].replace("+", " ")
email = data_dict["email"].replace("%40", "@")
number = data_dict["number"]
password = data_dict["password"]
cryped_password = '*' * len(password)

# Print HTML
print("<!DOCTYPE html><html><body>")
print(f"<h1>Hello {first_name}!</h1>")
print(f"<p>First name: {first_name}</p>")
print(f"<p>Last name: {last_name}</p>")
print(f"<p>Email: {email}</p>")
print(f"<p>Phone: {number}</p>")
print(f"<p>Password: {cryped_password}</p>")
print("</html></body>")
