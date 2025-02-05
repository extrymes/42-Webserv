#!/usr/bin/python3

import os
import sys

# Récupération et validation du Content-Type
contentType = os.environ.get("Content-Type", "Unknown")
print(contentType)
boundary = None

if "=" in contentType:
    boundary = contentType.split("=")[1]
else:
    print("⚠️ Erreur: Content-Type ne contient pas '='")
    boundary = "Unknown"

# Récupération et validation du Content-Length
content_length_str = os.environ.get("Content-Length", "0")
print(content_length_str)

try:
    content_length = int(content_length_str)
except ValueError:
    print(f"⚠️ Erreur: Content-Length invalide ({content_length_str})")
    content_length = 0

# Lecture du corps de la requête
# if content_length > 0:
#     # body = sys.stdin.buffer.read(content_length)
#     print(f"🔹 Body reçu ({len(body)} octets) :\n", body)
# else:
#     print("⚠️ Aucun body reçu ou Content-Length invalide")
