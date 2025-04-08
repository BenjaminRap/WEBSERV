FROM ubuntu
WORKDIR /
RUN [ "apt-get", "-y", "update" ]
RUN [ "apt-get", "-y", "upgrade" ]
RUN [ "apt-get", "-y", "install", "valgrind" ]
ENTRYPOINT [ "valgrind", "./WebServ" ]
CMD [ "./configuration.conf" ]
EXPOSE 8080
