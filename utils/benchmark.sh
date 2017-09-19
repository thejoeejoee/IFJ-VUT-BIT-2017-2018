#!/usr/bin/env bash
$TRAVIS_BUILD_DIR/ifj2017_benchmark --benchmark_format=json > $TRAVIS_BUILD_DIR/benchmark_results.json
USER=$USER API_KEY=$API_KEY /utils/plotly/plotly_upload_results.py $TRAVIS_BUILD_NUMBER $TRAVIS_BUILD_DIR/benchmark_results.json