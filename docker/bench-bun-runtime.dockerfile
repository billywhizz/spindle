FROM debian:bookworm-slim
RUN apt update -y
RUN apt install -y curl make g++ unzip
WORKDIR /spindle
RUN curl -L -o perf.tar.gz https://github.com/billywhizz/spindle/archive/refs/tags/perf.tar.gz
RUN tar -zxvf perf.tar.gz
RUN make -C spindle-perf full
RUN cp spindle-perf/spin ./spindle-full
RUN make -C spindle-perf minimal
RUN cp spindle-perf/spin ./spindle-minimal
RUN make -C spindle-perf minimal-static
RUN cp spindle-perf/spin ./spindle-minimal-static
RUN curl https://bun.sh/install | bash
RUN cp /root/.bun/bin/bun ./
RUN cp spindle-perf/bench/version.js ./
RUN curl -L -o hyperfine.tar.gz https://github.com/sharkdp/hyperfine/releases/download/v1.14.0/hyperfine-v1.14.0-x86_64-unknown-linux-gnu.tar.gz
RUN tar -zxvf hyperfine.tar.gz
RUN cp hyperfine-v1.14.0-x86_64-unknown-linux-gnu/hyperfine ./
CMD ["/bin/bash"]
