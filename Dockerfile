FROM lilian1024/opencv:4.14.0

#Prepare and build sentinelle
WORKDIR /app/sentinelle-src

RUN ["apt", "install", "-y", "gdb"]

COPY ./src ./src
COPY ./CMakeLists.txt ./CMakeLists.txt

RUN ["mkdir", "build"]

WORKDIR /app/sentinelle-src

RUN ["cmake", "-D", "CMAKE_BUILD_TYPE=DEBUG", "-B", "build"]
RUN ["cmake", "--build", "build"]

WORKDIR /app/sentinelle

RUN ["cp", "/app/sentinelle-src/build/Sentinelle", "./"]
RUN ["rm", "-r", "/app/sentinelle-src"]

#Setup and run app
RUN ["mkdir", "/data"]

COPY ./example/config.yml /data/config.yml

ENV SENTINELLE_SETTINGS_FILE=/data/config.yml

VOLUME [ "/data" ]

ENTRYPOINT [ "/app/sentinelle/Sentinelle" ]
