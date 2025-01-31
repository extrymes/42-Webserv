#!/usr/bin/python3

# Import OS module
import os

# Get data from fields
first_name = os.environ.get("first_name", "Unknown").replace("+", " ")
last_name = os.environ.get("last_name", "Unknown").replace("+", " ")
email = os.environ.get("email", "Unknown").replace("%40", "@")
number = os.environ.get("number", "Unknown")
password = os.environ.get("password", "Unknown")
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
