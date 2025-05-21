#!/usr/bin/env python3

import os

print("Content-Type: text/html")
print()  # Ligne vide obligatoire entre headers et corps

print("<html>")
print("<head><title>Test CGI</title></head>")
print("<body>")
print("<h1>Hello depuis un script CGI Python !</h1>")
print("<p>Voici quelques variables d'environnement :</p>")
print("<ul>")
for key in ("REQUEST_METHOD", "QUERY_STRING", "CONTENT_TYPE", "CONTENT_LENGTH"):
    print(f"<li>{key}: {os.environ.get(key, '(non défini)')}</li>")
print("</ul>")
print("</body>")
print("</html>")
