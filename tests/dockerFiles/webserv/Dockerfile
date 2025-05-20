FROM ubuntu
WORKDIR /
RUN [ "apt-get", "-y", "update" ]
RUN [ "apt-get", "-y", "upgrade" ]
RUN [ "apt-get", "-y", "install", "valgrind" ]
ENTRYPOINT [ "valgrind", "--leak-check=full", "--trace-children=yes", "--track-fds=yes", "./WebServ" ]
CMD [ "./configuration.conf" ]
EXPOSE 8080
