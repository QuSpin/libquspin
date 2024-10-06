ARG IMAGE_URL
FROM $IMAGE_URL

# Install the required packages
RUN apt-get update && apt-get install -y \
    python3 \
    python3-pip \
    python3-dev \
    build-essential

# Install the required python packages
RUN pip3 install meson ninja

# Set the working directory
WORKDIR /libquspin

# Copy the requirements file
COPY ../include/ /libquspin/include/
COPY ../src/ /libquspin/src/
COPY ../test/ /libquspin/test/
COPY ../meson.build /libquspin/

RUN meson setup builddir \ meson compile -C builddir
