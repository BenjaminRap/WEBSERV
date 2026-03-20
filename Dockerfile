FROM debian:bookworm-slim

RUN [ "apt-get", "update" ]
RUN [ "apt-get", "install", "-y", "git" ]
RUN [ "apt-get", "install", "-y", "make" ]
RUN [ "apt-get", "install", "-y", "g++" ]

WORKDIR /app

RUN [ "git", "clone", "https://github.com/BenjaminRap/WEBSERV.git", "webserv" ]

WORKDIR /app/webserv

RUN [ "make", "all" ]

ENTRYPOINT [ "./webserv", "./configuration.conf"]
