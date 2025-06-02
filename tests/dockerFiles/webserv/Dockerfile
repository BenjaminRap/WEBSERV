FROM ubuntu
WORKDIR /
RUN [ "apt-get", "-y", "update" ]
RUN [ "apt-get", "-y", "upgrade" ]
RUN [ "apt-get", "-y", "install", "valgrind" ]
RUN [ "apt-get", "-y", "install", "python3" ]
RUN [ "apt-get", "-y", "install", "php-cli" ]
# ENTRYPOINT [ "valgrind", "--leak-check=full", "--trace-children=yes", "--track-fds=yes", "./WebServ" ]
ENTRYPOINT [ "./WebServ" ]
CMD [ "./configuration.conf" ]
EXPOSE 8080
