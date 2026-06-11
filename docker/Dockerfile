# Wybranie wersji ROS-a
ARG ROS_DISTRO=jazzy
FROM ros:${ROS_DISTRO}-ros-base

# Blokada okienek instalatora i wymuszenie basha
ENV DEBIAN_FRONTEND=noninteractive
SHELL ["/bin/bash", "-c"]

# Instalacja nowszych sterowników graficznych do Qt i OpenGL
RUN apt-get update && \
    apt-get install -y software-properties-common && \
    add-apt-repository ppa:kisak/kisak-mesa -y

# Instalujemy podstawowe narzędzia oraz Twoje biblioteki: 
# CMake, Qt6 (z OpenGL), OpenCV, OpenGL i brakujące pakiety ROS 2 (TF2, cv_bridge)
RUN apt-get update && apt-get install -yq --no-install-recommends \
    wget \
    curl \
    git \
    build-essential \
    nano \
    htop \
    cmake \
    qt6-base-dev \
    libqt6opengl6-dev \
    libopencv-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    ros-${ROS_DISTRO}-cv-bridge \
    ros-${ROS_DISTRO}-tf2-ros \
    ros-${ROS_DISTRO}-tf2-geometry-msgs \
    && rm -rf /var/lib/apt/lists/*

# Usprawnienie życia: Auto-ładowanie ROS-a przy każdym otwarciu terminala
RUN echo "source /opt/ros/${ROS_DISTRO}/setup.bash" >> /root/.bashrc

# Ustawiamy katalog roboczy (tu zamontujemy Twój kod z Fedory)
WORKDIR /workspace

# 7. Domyślna komenda - po uruchomieniu trzymaj terminal otwarty
CMD ["/bin/bash"]