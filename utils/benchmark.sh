#!/usr/bin/env bash
if [ "${CC}" == "gcc" ]; then
    ${TRAVIS_BUILD_DIR}/build/ifj2017_benchmark --benchmark_format=json > ${TRAVIS_BUILD_DIR}/benchmark_results.json;
    USER=${PLOTLY_USER} API_KEY=${PLOTLY_API_KEY} ${TRAVIS_BUILD_DIR}/utils/plotly/plotly_upload_results.py ${TRAVIS_BRANCH} ${TRAVIS_BUILD_NUMBER} ${TRAVIS_BUILD_DIR}/benchmark_results.json
fi