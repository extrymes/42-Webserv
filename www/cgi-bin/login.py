#!/usr/bin/python3

# Import CGI module
import cgi

# Create instance of Field Storage
form = cgi.FieldStorage()

# Get data from fields
name = form.getvalue("username")
email = form.getvalue("email")
password = form.getvalue("password")

# Print headers
print("Content-Type: text/html\n")

# Print HTML
print("<html><body>")
print("<h1>Login form</h1>")
print(f"<p>Name: {name}</p>")
print(f"<p>Email: {email}</p>")
print(f"<p>Password: {password}</p>")
print("</html></body>")
