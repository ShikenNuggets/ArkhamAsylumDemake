FROM ghcr.io/ps2dev/ps2dev:latest

RUN apk add --no-cache cmake build-base ninja git bash

WORKDIR /root

RUN git clone https://github.com/ShikenNuggets/GadgetCore
RUN git clone https://github.com/ShikenNuggets/ArkhamAsylumDemake

CMD ["bash"]