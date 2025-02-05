#!/usr/bin/python3

import os
import linecache as lc 

contentType = os.environ.get("Content-Type", "Unknown")
boundary = contentType.split("=")[1]

body = os.environ.get("body", "Unknown").split(boundary)[1]

startFile = body.find('filename="') + len('filename="')
endFile = body.index('"', startFile)
filename = body[startFile:endFile].replace(' ', '_')

print(body)
# print(filename)

startType = body.find('Content-Type: ') + len('Content-Type: ')
endType = body.index('\r', startType)
contentType = body[startType:]

# print(contentType)

end_data = body.rfind("--")

data = body[endType:end_data]

with open("www/upload/" + filename, 'wb') as file:
	string = data.encode('utf-8', 'backslashreplace')
	file.write(string)