FROM ubuntu
WORKDIR /
RUN [ "apt-get", "-y", "update" ]
RUN [ "apt-get", "-y", "upgrade" ]
RUN [ "apt-get", "-y", "install", "valgrind" ]
# ENTRYPOINT [ "valgrind", "--leak-check=full", "--show-leak-kinds=all", "./WebServ" ]
ENTRYPOINT [ "valgrind", "--leak-check=full", "./WebServ" ]
# ENTRYPOINT [ "valgrind", "./WebServ" ]
CMD [ "./configuration.conf" ]
EXPOSE 8080
