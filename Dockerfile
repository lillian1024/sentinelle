FROM lilian1024/opencv:4.x

#Prepare and build sentinelle
WORKDIR /app/sentinelle-src

RUN ["apt", "install", "-y", "gdb", "libjsoncpp-dev", "libyaml-cpp-dev", "libmosquitto-dev"]

#Install correct version of g++ for ubuntu 24.x
#RUN ["apt", "install", "-y", "software-properties-common", "gcc-14", "g++-14"]
#RUN ["add-apt-repository", "-y", "ppa:ubuntu-toolchain-r/test"]
#RUN ["update-alternatives", "--install", "/usr/bin/g++", "g++", "/usr/bin/g++-14", "100"]
#RUN ["update-alternatives", "--install", "/usr/bin/gcc", "gcc", "/usr/bin/gcc-14", "100"]

COPY ./src ./src
COPY ./CMakeLists.txt ./CMakeLists.txt

RUN ["mkdir", "build"]

WORKDIR /app/sentinelle-src

RUN ["cmake", "-D", "CMAKE_BUILD_TYPE=DEBUG", "-B", "build"]
RUN ["cmake", "--build", "build", "-j", "18"]

WORKDIR /app/sentinelle

RUN ["cp", "/app/sentinelle-src/build/Sentinelle", "./"]
RUN ["rm", "-r", "/app/sentinelle-src"]

#Setup and run app
RUN ["mkdir", "/data"]

COPY ./example/config.yml /data/config.yml

ENV SENTINELLE_SETTINGS_FILE=/data/config.yml

VOLUME [ "/data" ]

ENTRYPOINT [ "/app/sentinelle/Sentinelle" ]
