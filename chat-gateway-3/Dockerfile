FROM ubuntu:22.04 AS builder

WORKDIR /app

COPY . .

RUN apt-get update &&\
    apt-get install -y g++ make cmake &&\
    cmake -S . -B build && cmake --build build &&\
    rm -rf /var/lib/apt/lists/*

FROM ubuntu:22.04

WORKDIR /app

COPY --from=builder /app/build/gateway /app/gateway
COPY --from=builder /app/config /app/config
EXPOSE 9001
CMD ["./gateway", "--config=/app/config/gateway.conf", "--port=9001"]
