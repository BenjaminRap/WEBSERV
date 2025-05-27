#!/usr/bin/env python3

import os

print("Content-Type: text/html", end="\r\n")
print("", end="\r\n")  # Ligne vide obligatoire entre headers et corps

print("<html>", end="\r\n")
print("<head><title>Test CGI</title></head>", end="\r\n")
print("<body>", end="\r\n")
print("<h1>Hello depuis un script CGI Python !</h1>", end="\r\n")
print("<p>Voici quelques variables d'environnement :</p>", end="\r\n")
print("<ul>", end="\r\n")
for key in ("REQUEST_METHOD", "QUERY_STRING", "CONTENT_TYPE", "CONTENT_LENGTH"):
    print(f"<li>{key}: {os.environ.get(key, '(non défini)')}</li>", end="\r\n")
print("</ul>", end="\r\n")
print("</body>", end="\r\n")
print("</html>", end="\r\n")
