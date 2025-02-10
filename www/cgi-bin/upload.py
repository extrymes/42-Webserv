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
		filename = content.split(b"filename=\"")[1].split(b"\"")[0].decode()
		file_content = content.split(b"\r\n\r\n")[1]
		if os.path.exists(upload_location):
			with open(upload_location + filename, 'wb') as file:
				file.write(file_content)
				body = "File succesfully uploaded!"
		else:
			body = "Invalid upload save location!"
		body_length = len(body)
		print("Content-Type: text/plain\r")
		print(f"Content-Length: {body_length}\r\n\r")
		print(f"{body}")
		sys.exit(0)

sys.exit(1)
