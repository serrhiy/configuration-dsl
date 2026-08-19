# syntax=docker/dockerfile:1

FROM gcc:trixie

ARG USER_ID=1000
ARG USER_NAME=dev

RUN apt update \
    && apt install -y sudo cmake git gdb ninja-build clang-format-19 curl zip unzip tar

RUN useradd -ms /bin/bash -u ${USER_ID} ${USER_NAME}
RUN echo "${USER_NAME} ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers
USER ${USER_NAME}

# Setup vcpkg
ENV VCPKG_ROOT=/home/${USER_NAME}/vcpkg
ENV PATH="$VCPKG_ROOT:$PATH"
RUN git clone https://github.com/microsoft/vcpkg.git $VCPKG_ROOT \
    && $VCPKG_ROOT/bootstrap-vcpkg.sh -disableMetrics

# Setup GDB
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
