FROM node:20-bullseye
WORKDIR /
RUN [ "apt-get", "-y", "update" ]
RUN [ "apt-get", "-y", "upgrade" ]
RUN [ "apt-get", "-y", "install", "zsh" ]
RUN [ "npm", "install", "-g", "http-parser-js" ]
ENTRYPOINT [ "zsh" ]
