# Install dependencies

## Build

```bash
docker build . -t mandelbrot-render-utils:latest
```

## Install libs

Libs from apt

```bash
sudo apt-get update && apt-get install -y --no-install-recommends \
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
```

Spdlog

```bash
git clone https://github.com/gabime/spdlog.git \
	&& cd spdlog \
	&& git checkout v1.10.0 \
	&& mkdir .build \
	&& cd .build \
	&& cmake -DSPDLOG_BUILD_TESTS=OFF -DSPDLOG_BUILD_EXAMPLE=OFF -DSPDLOG_BUILD_BENCH=OFF -DSPDLOG_FMT_EXTERNAL=OFF -DSPDLOG_BUILD_SHARED=ON .. \
	&& make -j 12 \
	&& sudo make install \
	&& cd ../..  \
    && rm -rf spdlog
```

Libfmt

```bash
git clone https://github.com/fmtlib/fmt.git \
    && cd fmt \
    && git checkout 8.1.1 \
    && mkdir .build \
    && cd .build \
    && cmake -DFMT_TEST=OFF .. \
    && make -j 12 \
    && sudo make install \
    && cd ../.. \
    && rm -rf fmt
```

Cxxopts

```bash
git clone https://github.com/jarro2783/cxxopts.git \
    && cd cxxopts \
    && git checkout v3.1.1 \
    && mkdir build \
    && cd build \
    && cmake -DCXXOPTS_BUILD_TESTS=OFF  -DCXXOPTS_BUILD_EXAMPLES=OFF .. \
    && sudo cmake --build . --target install -- -j 12 \
    && cd ../.. \
    && rm -rf cxxopts
```

## Build

```bash
mkdir "build"  \
    && cd build  \
    && cmake ..  \
    && cmake --build . -- -j 12
```

## Run

Image render with C++
```bash
./render_mandelbrot_opencv_img -p "mandelbrot.png" -i 50 -t 50 --imin "-1.1" --imax 1.1 --rmin "-2.5" --rmax="1.0"
```