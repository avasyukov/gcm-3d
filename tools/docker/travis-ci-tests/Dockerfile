FROM fufler/gcm3d-travis-ci-env:f22

COPY gcm-3d /root/gcm-3d
COPY run_tests.sh /root/gcm-3d/run_tests.sh

RUN                                              \
    cd /root/gcm-3d                           && \
    mkdir build                               && \
    cd build                                  && \
    export PATH=/usr/lib64/openmpi/bin/:$PATH && \
    cmake -DCMAKE_BUILD_TYPE=RELEASE ..       && \
    make -j2
