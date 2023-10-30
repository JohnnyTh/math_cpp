FROM ubuntu:22.04

ENV DEBIAN_FRONTEND noninteractive

# ca-certificates - install to avoid error server certificate verification failed. CAfile: none CRLfile: none
# when running git clone
RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates \
    libopencv-dev \
    libglu1-mesa-dev \
    freeglut3-dev \
    mesa-common-dev \
    libglew-dev \
    libglfw3 \
    libglfw3-dev \
    git \
    make \
    cmake \
    gcc \
    g++

# Install spdlog
RUN git clone https://github.com/gabime/spdlog.git \
	&& cd spdlog \
	&& git checkout v1.10.0 \
	&& mkdir .build \
	&& cd .build \
	&& cmake -DSPDLOG_BUILD_TESTS=OFF -DSPDLOG_BUILD_EXAMPLE=OFF -DSPDLOG_BUILD_BENCH=OFF -DSPDLOG_FMT_EXTERNAL=OFF -DSPDLOG_BUILD_SHARED=ON .. \
	&& make -j 12 \
	&& make install \
	&& cd ../..  \
    && rm -rf spdlog

# Install cxxopts
RUN git clone https://github.com/jarro2783/cxxopts.git \
    && cd cxxopts \
    && git checkout v3.1.1 \
    && mkdir build \
    && cd build \
    && cmake -DCXXOPTS_BUILD_TESTS=OFF  -DCXXOPTS_BUILD_EXAMPLES=OFF .. \
    && cmake --build . --target install -- -j 12 \
    && cd ../.. \
    && rm -rf cxxopts

# RUN apt-get update && apt-get install -y libdl-dev

COPY . /math_cpp/
WORKDIR /math_cpp/

# build final executables
RUN mkdir "build"  \
    && cd build  \
    && cmake ..  \
    && cmake --build . -- -j 12  \
    && cp render_mandelbrot_opencv_img render_mandelbrot_opengl_shader ..

CMD ["/bin/bash"]