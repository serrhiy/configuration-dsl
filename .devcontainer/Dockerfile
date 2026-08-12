# syntax=docker/dockerfile:1

FROM gcc:trixie

ARG USER_ID=1000

RUN apt update
RUN apt install -y sudo cmake git gdb ninja-build clang-format-19
RUN echo "dev ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

RUN useradd -ms /bin/bash -u ${USER_ID} dev
USER dev