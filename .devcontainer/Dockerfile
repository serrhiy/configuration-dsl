# syntax=docker/dockerfile:1

FROM gcc:trixie

ARG USER_ID=1000

RUN apt update && apt install -y sudo cmake git gdb ninja-build clang-format-19

RUN useradd -ms /bin/bash -u ${USER_ID} dev
RUN echo "dev ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers
USER dev

RUN cat > /home/dev/.gdbinit <<'EOF'
set auto-load safe-path /
set print pretty on
set print elements 200

skip -rfu ^std::
skip -rfu ^__gnu_cxx::
skip -gfi /usr/local/include/c++/*/*
skip -gfi /usr/local/include/c++/*/*/*
skip -gfi /usr/local/include/c++/*/*/*/*
EOF
