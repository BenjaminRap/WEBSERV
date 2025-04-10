.IGNORE:clean, build-webserv, restart-webserv-container, test
.SILENT:clean, build-webserv, restart-webserv-container, test

DOCKER_DIR	=	./dockerFiles/
DOCKER_FILE	=	docker-compose.yaml
DOCKER_EXEC	=	docker compose -f $(DOCKER_DIR)$(DOCKER_FILE)

restart-webserv-container: build-webserv
	$(DOCKER_EXEC) restart --no-deps webserv

build-webserv:
	cd ../code && make all -j16

build-containers:
	$(DOCKER_EXEC) build

up:
	$(DOCKER_EXEC) up -d
	$(DOCKER_EXEC) logs -f nginx > ./dockerFiles/nginx/nginx.logs &
	$(DOCKER_EXEC) logs -f webserv > ./dockerFiles/webserv/webserv.logs &

test: build-webserv build-containers up
	$(DOCKER_EXEC) attach tests

all: build-webserv build-containers up

stop:
	$(DOCKER_EXEC) stop

clean: stop
	docker stop $(docker ps -qa) 2>/dev/null
	docker rm $(docker ps -qa) 2>/dev/null
	docker rmi -f $(docker images -qa) 2>/dev/null
	docker volume rm $(docker volume ls -q) 2>/dev/null
	docker network rm $(docker network ls -q) 2>/dev/null

fclean: clean
	docker system prune -af

re: clean all
fre: fclean all
