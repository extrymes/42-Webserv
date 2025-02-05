#!/usr/bin/python3

# import os
# import sys
# # import linecache as lc 

# contentType = os.environ.get("Content-Type", "Unknown")
print("bonjour")
# boundary = contentType.split("=")[1]

# content_length = int(os.environ.get("Content-Length", "Unknown"))
# print(content_length)
# body = sys.stdin.buffer.read(content_length)

# print(body)
# body = os.environ.get("body", "Unknown").split(boundary)[1]

# startFile = body.find('filename="') + len('filename="')
# endFile = body.index('"', startFile)
# filename = body[startFile:endFile].replace(' ', '_')

# print(filename)

# startType = body.find('Content-Type: ') + len('Content-Type: ')
# endType = body.index('\r', startType)
# contentType = body[startType:endType]

# # print(contentType)

# end_data = body.rfind("--")

# data = body[endType:end_data]

# with open("www/upload/" + filename, 'wb') as file:
# 	string = data.encode('utf-8', 'backslashreplace')
# 	file.write(string)