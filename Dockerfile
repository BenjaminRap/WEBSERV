FROM debian:bookworm-slim

WORKDIR /app

RUN [ "git", "clone", "https://github.com/BenjaminRap/WEBSERV.git", "webserv" ]

WORKDIR /app/webserv/code
