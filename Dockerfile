FROM debian:bookworm-slim

RUN [ "apt-get", "update" ]
RUN [ "apt-get", "install", "-y", "git" ]
RUN [ "apt-get", "install", "-y", "make" ]
RUN [ "apt-get", "install", "-y", "g++" ]

WORKDIR /app

RUN [ "git", "clone", "https://github.com/BenjaminRap/WEBSERV.git", "webserv" ]

WORKDIR /app/webserv

RUN [ "make", "all" ]

FROM debian:bookworm-slim

WORKDIR /app

RUN [ "apt-get", "update" ]
RUN [ "apt-get", "install", "-y", "python3" ]
RUN [ "apt-get", "install", "-y", "php-cli" ]

COPY --from=0 /app/webserv/WebServ /app/WebServ

ENTRYPOINT [ "./WebServ", "./configuration.conf" ]
