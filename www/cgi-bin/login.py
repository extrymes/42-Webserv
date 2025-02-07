#!/usr/bin/python3

# Import OS module
import os

def print_html(first_name, last_name, email, number, password):
	print("<!DOCTYPE html><html><body>")
	if first_name:
		print(f"<h1>Hello {first_name}!</h1>")
		print(f"<p>First name: {first_name}</p>")
		print(f"<p>Last name: {last_name}</p>")
		print(f"<p>Email: {email}</p>")
		print(f"<p>Phone: {number}</p>")
		print(f"<p>Password: {password}</p>")
	else:
		print("<h1>Hello Guest!</h1>")
	print("</html></body>")


# Get body in environment
body = os.environ.get("body")
if not body:
	# Print HTML
	print_html(None, None, None, None, None)
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
first_name = data_dict["first_name"].replace("+", " ")
first_name = first_name
if first_name:
	first_name = first_name.replace("+", " ")
last_name = data_dict["last_name"]
if last_name:
	last_name = last_name.replace("+", " ")
email = data_dict["email"].replace("%40", "@")
if email:
	email = email.replace("%40", "@")
number = data_dict["number"]
password = data_dict["password"]
cryped_password = '*' * len(password)

# Print HTML
print_html(first_name, last_name, email, number, cryped_password)
