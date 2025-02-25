telnet 127.0.0.1 8181 << EOF
PUT /uploads/test.html HTTP/1.1
Host: foo.com
Content-Type: text/html
Content-Length: 16
Connection: keep-alive

say=Hi&
EOF

telnet 127.0.0.1 8181 << EOF
DELETE /uploads/test.html HTTP/1.1
Host: foo.com
Connection: keep-alive

say=Hi&to=MomAnd
EOF