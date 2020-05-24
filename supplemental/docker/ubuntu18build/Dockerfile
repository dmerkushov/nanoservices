FROM ubuntu:18.04
LABEL name="ubuntu18_ns_devel"
LABEL version="v0.1.0"

ENV PACKAGES="sudo apt-utils curl git software-properties-common build-essential software-properties-common gcc-6 g++-6 cmake make libzookeeper-mt-dev zookeeper vim nano"
SHELL ["/bin/bash", "-c"]
RUN \
    apt-get update -q; \
    apt-get upgrade -y -q;\
    apt-get install -y -q $PACKAGES; \
    adduser --disabled-password --gecos '' deploy; \
    adduser deploy sudo; \
    echo "%sudo ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers ; \
    echo "Set disable_coredump false" >> /etc/sudo.conf ; \
    apt-get autoclean


USER deploy
RUN \
    cd ~; \
    git clone https://github.com/dmerkushov/nanoservices.git; \
    cd nanoservices; \
    git checkout ag-develop; \ 
    mkdir build; \
    cd build; \
    cmake .. ;\
    sudo /usr/share/zookeeper/bin/zkServer.sh start; \
    cmake --build . ;\
    cpack ;\
    echo "echo 'Please start zookeper before you go further:'" >> ~/.bashrc ;\
    echo "sudo /usr/share/zookeeper/bin/zkServer.sh start" >> ~/.bashrc