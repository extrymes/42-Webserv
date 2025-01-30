#!/usr/bin/python3

# Import OS module
import os

# Get data from fields
first_name = os.environ.get("first_name", "Unknown")
last_name = os.environ.get("last_name", "Unknown")
email = os.environ.get("email", "Unknown")
password = os.environ.get("password", "Unknown")

# Print HTML
print("<html><body>")
print(f"<h1>Hello {first_name}!</h1>")
print(f"<p>Firstname: {first_name}</p>")
print(f"<p>Lastname: {last_name}</p>")
print(f"<p>Email: {email}</p>")
print(f"<p>Password: {password}</p>")
print("</html></body>")
