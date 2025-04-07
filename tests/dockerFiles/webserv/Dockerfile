FROM ubuntu
WORKDIR /
RUN [ "apt-get", "-y", "update" ]
RUN [ "apt-get", "-y", "upgrade" ]
RUN [ "apt-get", "-y", "install", "valgrind" ]
COPY ./configuration.conf ./configuration.conf
COPY ./WebServ ./WebServ
ENTRYPOINT [ "valgrind", "./WebServ" ]
CMD [ "./configuration.conf" ]
EXPOSE 8080
