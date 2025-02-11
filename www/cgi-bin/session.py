#!/usr/bin/python3

import os
# import http.cookies
import uuid
import requests
# import browser_cookie3

SESSION_DIR = "/tmp/cgi_sessions"

class Session:
	def __init__(self):
		self.session_id = None
		self.data = {}
		# Ensure sessions directory exis ts
		if not os.path.exists(SESSION_DIR):
			os.makedirs(SESSION_DIR, mode=0o700)

	def load_session(self):
		# print(os.environ)
		# host = "http://" + os.environ.get("host") + ":" + os.environ.get("port")
		# Retrieve environment variables
		host = os.environ.get("host")
		port = os.environ.get("port")

		# Check if environment variables are set
		if not host or not port:
			print("Environment variables 'host' and 'port' must be set.")
			return

		host = f"http://{host}:{port}"

		try:
			# send HTTP requests
			response = requests.get("http://localhost:8083", timeout=2.50)
			# print out Cookies
			for cookie in response.cookies:
				print(cookie.name, cookie.value)
		except requests.exceptions.RequestException as e:
			print(f"An error occurred: {e}")

	def create_new_session(self):
		# Create new session
		self.session_id = str(uuid.uuid4())
		self.data = {}
		self.save_session()

	def save_session(self):
		# Save session data to a file
		session_file = os.path.join(SESSION_DIR, self.session_id)
		with open(session_file, "w") as f:
			for key, value in self.data.items():
				f.write(f"{key}={value}\n")

	def set(self, key, value):
		# Set session variable
		self.data[key] = value
		self.save_session()

	def get(self, key):
		# Return session variable
		return self.data.get(key)

	def get_cookie_header(self):
		# Return the Set-Cookie header for new sessions
		if self.session_id:
			return f"SESSION_ID={self.session_id}; Path=/; HttpOnly;"
		else:
			return ""
