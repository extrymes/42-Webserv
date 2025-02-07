#!/usr/bin/python3

import os
import sys

content_type = os.environ.get("Content-Type", "Unknown")
upload_location = os.environ.get("upload_location", "www/upload/")

boundary = content_type.split("boundary=")[-1]
boundary_b = b"\r\n--" + boundary.encode()
input_data = sys.stdin.buffer.read()

split_input = input_data.split(boundary_b)

for content in split_input:
	if b"filename" in content:
		# sys.stderr.write("ok1\n")
		filename = content.split(b"filename=\"")[1].split(b"\"")[0].decode()
		file_content = content.split(b"\r\n\r\n")[1]

		# file_path = '.' + os.path.join(os.path.dirname(__file__), filename)
		# os.makedirs(os.path.dirname(file_path), exist_ok=True)
		with open(upload_location + filename, 'wb') as file:
			file.write(file_content)

		body = "File succesfully uploaded !"
		body_length = len(body)

		# sys.stderr.write(f"{body}\r")
		print("Content-Type: text/plain\r")
		print(f"Content-Length: {body_length}\r\n\r")
		print(f"{body}")
		sys.exit(0)

sys.exit(1)
