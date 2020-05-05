# NANOSERVICES DOCKER IMAGE

## Build docker image

```bash
cd nanoservices

docker build --pull --rm -f "supplemental/docker/ubuntu18build/Dockerfile" -t nanoservices:v$(cat VERSION.txt) -t nanoservices:latest "supplemental/docker/ubuntu18build"
```

While building you will see that:

1. Dependencies are installed

1. GIT clone nanoservice sources

1. CMAKE builds it up

1. Tests run successfull

1. DEB packages are created

## Run Docker Container

```bash
cd nanoservices/supplemental/docker/ubuntu18build

docker-compose up &
```

To attach shell run:

```bash
docker container list #look at container id for image nanoservices:v***

docker exec -it $CONTAINER_ID /bin/sh -c "[ -e /bin/bash ] && /bin/bash || /bin/sh"
```

## Keep in mind

This Dockerfile builds nanoservices with .deb packeges and zookeeper configuration libraries.

```bash
build
|-- nanoservices-zoo-dev_6.2.1_all.deb      # header files
|-- nanoservices-zoo-lib_6.2.1_amd64.deb    # libraries
|-- nanoservices-zoo-misc_6.2.1_amd64.deb   # tools like configurator and shutdown
```

If needed you should change Dockerfile in:

```yaml
USER deploy
RUN \
    cd ~; \
    git clone https://github.com/dmerkushov/nanoservices.git; \
    cd nanoservices; \
    git checkout ag-develop; \  # TODO remove checkout
    mkdir build; \
    cd build; \
    cmake .. ;\
    sudo /usr/share/zookeeper/bin/zkServer.sh start; \ # need for testing purposes
    cmake --build . ;\
    cpack ;\
    echo "echo 'Please start zookeper before you go further:'" >> ~/.bashrc ;\
    echo "sudo /usr/share/zookeeper/bin/zkServer.sh start" >> ~/.bashrc # you will need zookeper when starts docker container
```

## Versioning TAG

Version of the Dockerimage should be the same as nanoservices VERSION.txt

## BUG

If you'll get a bug when Docker image builds don't hesitate to issue
