CONTAINER_NAME:=shriek
BASE_IMAGE:=ubuntu

### Dockerized Linux workspace for consistent environment
docker-clean:
	-docker stop $(CONTAINER_NAME)
	-docker rm $(CONTAINER_NAME)

docker-pull:
	docker pull $(BASE_IMAGE)

container:
	docker run \
	-dt \
	--name $(CONTAINER_NAME) \
	-p 6666:6666 \
	-v `pwd`:/$(CONTAINER_NAME) \
	$(BASE_IMAGE)

docker-install:
	docker exec $(CONTAINER_NAME) apt-get update
	docker exec $(CONTAINER_NAME) apt-get install -y make valgrind clang clang-tools cdecl perl bsdmainutils netcat net-tools
	docker exec $(CONTAINER_NAME)

shell:
	docker exec -it $(CONTAINER_NAME) /bin/bash

workspace: docker-clean docker-pull container docker-install shell
