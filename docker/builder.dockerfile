FROM debian:bookworm-slim
RUN apt update -y
RUN apt install -y curl make g++ autoconf libtool
WORKDIR /spindle
RUN curl -L -o perf.tar.gz https://github.com/billywhizz/spindle/archive/refs/tags/perf.tar.gz
RUN tar -zxvf perf.tar.gz
RUN cd spindle-perf
RUN make full
RUN make MODULE=ffi library
RUN make MODULE=tcc library
CMD ["/bin/bash"]
