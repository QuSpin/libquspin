ARG IMAGE_URL
FROM $IMAGE_URL

WORKDIR /usr/libquspin/

RUN mkdir -p /data

# Install the required python packages
RUN manylinux-interpreters ensure cp312-cp312
RUN python3 -m venv .venv
RUN source .venv/bin/activate
RUN python3 -m ensurepip
RUN python3 -m pip install meson ninja

# Copy the requirements file
COPY . ./

# RUN meson setup builddir --buildtype=release --reconfigure
# RUN meson compile -C builddir -v -j 1
# RUN meson test -C builddir

RUN echo "test test test" > test.txt
RUN ls -la
# COPY --from=context test.txt /artifacts
