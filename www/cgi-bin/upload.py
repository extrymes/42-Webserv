#!/usr/bin/python3

import os
import sys
# # import linecache as lc 

contentType = os.environ.get("Content-Type", "Unknown")
boundary = contentType.split("=")[1]

body = sys.stdin.buffer.read()

body = body.split(boundary.encode())[1]

startFile = body.find(b'filename="') + len(b'filename="')
endFile = body.index(b'"', startFile)
filename = body[startFile:endFile].replace(b' ', b'_')

# print(filename)

startType = body.find(b'Content-Type: ') + len(b'Content-Type: ')
endType = body.index(b'\r', startType)
contentType = body[startType:endType]

# print(contentType)

endData = body.rfind(b"--")

# print(end_data)

data = body[endType:endData]

print(data)

with open("www/upload/" + filename.decode(), 'wb') as file:
	file.write(data)


# content_length = os.getenv('Content-Length')
# content_type = os.getenv('Content-Type')
# # upload_save = os.getenv('UPLOAD_SAVE')

# if ((not content_length) or (not content_type)):
# 	sys.exit(1)

# boundary = content_type.split("=")[-1]
# boundary_b = b"\r\n--" + boundary.encode()
# input_data = sys.stdin.buffer.read()

# # print(input_data)

# split_input = input_data.split(boundary_b)