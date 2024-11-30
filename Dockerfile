# Use a Debian base image
FROM debian:bullseye

# Set non-interactive mode for apt-get
ENV DEBIAN_FRONTEND=noninteractive

# Install required dependencies for devkitARM
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    wget \
    git \
    python3 \
    python3-pip \
    libncurses5 \
    libtinfo5 \
    && apt-get clean

# Install devkitPro pacman
RUN wget https://apt.devkitpro.org/install-devkitpro-pacman && \
    chmod +x install-devkitpro-pacman && \
    yes | ./install-devkitpro-pacman && \
    rm install-devkitpro-pacman

# Add devkitARM using devkitPro pacman
RUN dkp-pacman -Sy --noconfirm 3ds-dev

# Set environment variables for devkitARM
ENV DEVKITPRO=/opt/devkitpro
ENV DEVKITARM=$DEVKITPRO/devkitARM
ENV PATH=$PATH:$DEVKITPRO/tools/bin

# Create a working directory
WORKDIR /workspace

# Run cmake to configure and build the project
ENTRYPOINT ["bash", "-c", "cmake -DCMAKE_BUILD_TYPE=Debug -DPLATFORM=N3DS -B build . && cmake --build build"]