.IGNORE:clean
.SILENT:clean

DOCKER_DIR	=	./dockerFiles/
DOCKER_FILE	=	docker-compose.yaml
DOCKER_EXEC	=	docker compose -f $(DOCKER_DIR)$(DOCKER_FILE)

build:
	$(DOCKER_EXEC) build

up:
	$(DOCKER_EXEC) up

all: build up

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