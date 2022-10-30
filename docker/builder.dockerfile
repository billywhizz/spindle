FROM debian:bookworm-slim
RUN apt update
RUN apt install -y curl make g++ autoconf libtool
WORKDIR /spin
RUN curl -L -o perf.tar.gz https://github.com/billywhizz/spin/archive/refs/tags/perf.tar.gz
RUN tar -zxvf perf.tar.gz
RUN cd spin-perf
RUN make full
CMD ["/bin/bash"]
