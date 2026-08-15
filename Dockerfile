FROM nvidia/cuda:13.3.1-cudnn-devel-ubuntu26.04

WORKDIR /tmp

RUN ["apt", "update"]
RUN ["apt", "upgrade", "-y"]

RUN ["apt", "install", "-y", "gcc", "cmake", "git"]

#Installing dependencies
RUN ["apt", "install", "-y", "ffmpeg", "libavcodec-dev", "libavformat-dev", "libavutil-dev", "libswscale-dev", "libavdevice-dev"]

RUN ["git", "clone", "https://github.com/opencv/opencv.git"]
RUN ["git", "clone", "https://github.com/opencv/opencv_contrib.git"]

RUN ["cd", "opencv"]
RUN ["mkdir", "build"]

#Config and build OpenCV
RUN ["cmake", "CMAKE_BUILD_TYPE=RELEASE", "-D OPENCV_EXTRA_MODULES_PATH=../opencv_contrib/modules", "-D WITH_CUDA=ON", "-D WITH_CUDNN=ON", "-D OPENCV_DNN_CUDA=ON", "-D ENABLE_FAST_MATH=ON", "-D CUDA_FAST_MATH=ON", "-D WITH_CUBLAS=ON", "-D CUDA_ARCH_BIN=8.6", "-D CUDNN_INCLUDE_DIR=/usr/local/cuda/include", "-D CUDNN_LIBRARY=/usr/local/cuda/lib64/libcudnn.so", "-D BUILD_opencv_python3=ON", "-D WITH_FFMPEG=ON", "-B", "build"]
RUN ["cmake", "--build", "build"]

#install OpenCV
RUN ["make", "-C", "build", "install"]

RUN ["cd", "/tmp"]

RUN ["rm", "-r", "opencv", "opencv_contrib"]

#Prepare and build sentinelle
WORKDIR /app/sentinelle-src

COPY ./src ./src
COPY ./CMakeLists.txt ./CMakeLists.txt

RUN ["cmake", "-B", "build"]
RUN ["cmake", "-build", "build"]

WORKDIR /app/sentinelle

RUN ["cp", "/app/sentinelle-src/build/Sentinelle", "./"]
RUN ["rm", "-r", "/app/sentinelle-src"]

#Setup and run app
RUN ["mkdir", "/data"]

COPY ./example/config.yml /data/config.yml

ENV SENTINELLE_SETTINGS_FILE=/data/config.yml

VOLUME [ "/data" ]

ENTRYPOINT [ "/app/sentinelle/Sentinelle" ]
