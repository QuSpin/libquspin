ARG IMAGE_URL
FROM $IMAGE_URL

WORKDIR /usr/libquspin/

# Install the required python packages
RUN manylinux-interpreters ensure cp312-cp312
RUN python3 -m venv .venv
RUN source .venv/bin/activate
RUN python3 -m ensurepip
RUN python3 -m pip install meson ninja

# Copy the requirements file
COPY ../ ./

RUN meson setup builddir --buildtype=release --reconfigure
RUN meson compile -C builddir -v
RUN meson test -C builddir

# COPY /libquspin/buildir/libquspin.a /data/libquspin.a
