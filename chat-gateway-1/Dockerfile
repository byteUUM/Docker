FROM ubuntu:22.04 AS gateway
WORKDIR /app
COPY . .
RUN apt-get install updata &&\
    apt-get install -y g++ make cmake &&\
    
RUN cmake -S . -B build && cmake --build build

WORKDIR /app/build

CMD ["./gateway"]
