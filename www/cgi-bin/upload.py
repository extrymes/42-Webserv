#!/usr/bin/python3

import os
import sys

content_type = os.environ.get("Content-Type", "Unknown")
content_length = os.environ.get('Content-Lenght', "Unknown")

# sys.stderr.write(content_length)
# sys.stderr.write(content_type)

# if ((not content_length) or (not content_type)):
#     sys.exit(1)


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
        with open("www/upload/" + filename, 'wb') as file:
            file.write(file_content)

        body = "File succesfully uploaded !"
        body_length = len(body)

        sys.stderr.write(f"{body}\r")
        print("Content-Type: text/plain\r")
        print(f"Content-Length: {body_length}\r\n\r")
        print(f"{body}")
        sys.exit(0)

sys.exit(1)