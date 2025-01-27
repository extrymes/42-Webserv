#!/usr/bin/pyhon3

import os
import http.cookies
import uuid

SESSION_DIR = "/tmp/cgi_sessions"

class Session:
	def __init__(self):
		self.session_id = None
		self.data = {}
		# Ensure sessions directory exists
		if not os.path.exists(SESSION_DIR):
			os.makedirs(SESSION_DIR, mode=0o700)

	def load_session(self):
		# Load session from cookie or create new one
		cookie = http.cookies.SimpleCookie(os.environ.get("HTTP_COOKIE"))
		if "SESSION_ID" in cookie:
			self.session_id = cookie["SESSION_ID"].value
			session_file = os.path.join(SESSION_DIR, self.session_id)
			if os.path.exists(session_file):
				with open(session_file, "r") as f:
					for line in f:
						key, value = line.strip().split("=", 1)
						self.data[key] = value
		else:
			self.create_new_session()

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
			return f"Set-Cookie= SESSION_ID={self.session_id}; Path=/; HttpOnly"
		else:
			return ""
