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
		if not os.path.exists(upload_location):
			try:
				os.makedirs(upload_location)
			except:
				body = """
				<h3>Cannot create upload location!</h3>
				<iframe src="https://giphy.com/embed/v2JqIt9EQKMFb1bGUh" width="480" height="269" style="" frameBorder="0" class="giphy-embed" allowFullScreen></iframe>"""
		try:
			with open(upload_location + filename, 'wb') as file:
				file.write(file_content)
				body = """
				<h3>File succesfully uploaded!</h3>
				<iframe src="https://giphy.com/embed/VJY3zeoK87CLBKnqqm" width="480" height="480" style="" frameBorder="0" class="giphy-embed" allowFullScreen></iframe>"""
		except:
			body = """
			<h3>Cannot create upload location!</h3>
			<iframe src="https://giphy.com/embed/v2JqIt9EQKMFb1bGUh" width="480" height="269" style="" frameBorder="0" class="giphy-embed" allowFullScreen></iframe>"""

		print("HTTP/1.1 200 OK")
		print("Content-Type: text/html")
		print("Content-Length: ", len(body))
		print("Connection: close\r\n\r\n")
		print(f"{body}")
		sys.exit(0)

sys.exit(1)
