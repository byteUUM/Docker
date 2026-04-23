FROM ubuntu:22.04 AS builder
COPY . /app
WORKDIR /app
RUN apt-get update &&\
    apt-get install -y g++ make cmake &&\
    cmake -S . -B build && cmake --build build &&\
    rm -rf /var/bin/apt/lists/*
	
FROM ubuntu:22.04

WORKDIR /app
COPY --from=builder /app/build/gateway /app/gateway
EXPOSE 9001
CMD ["./gateway"]
 
