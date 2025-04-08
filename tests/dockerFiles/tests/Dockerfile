FROM node:20-bullseye
WORKDIR /
RUN [ "apt-get", "-y", "update" ]
RUN [ "apt-get", "-y", "upgrade" ]
RUN [ "apt-get", "-y", "install", "zsh" ]
ENTRYPOINT [ "zsh" ]
