#!/usr/bin/env python3
# coding=utf-8

import json
import os
import sys
from operator import itemgetter

import plotly
import plotly.graph_objs as go
import plotly.plotly as py
from plotly.exceptions import PlotlyError

PLOTLY_FILENAME = "IFJ_benchmark"


def parse_numeric(s):
    try:
        return int(s)
    except ValueError:
        return float(s)


def conv(v):
    if isinstance(v, list):
        return v
    elif isinstance(v, (int, float)):
        return [v]
    elif isinstance(v, str):
        return [parse_numeric(v)]


def plotly_combine(data_a, data_b):
    result = []
    names = set(tuple(map(itemgetter('name'), data_a)) + tuple(map(itemgetter('name'), data_b)))

    # combine data
    for name in names:
        xy_tuples = []
        tmp_data = {
            "type": "scatter",
            "name": name,  # name
            "hoverinfo": "x+y+name",
            "x": [],  # build number
            "y": []  # time
        }
        for item in tuple(data_a) + tuple(data_b):
            if "name" in item.keys() and item["name"] == name:
                xy_tuples.extend(zip(conv(item["x"]), conv(item["y"])))

        unified_tuples = list(dict(xy_tuples).items())
        unified_tuples.sort(key=itemgetter(0))

        if unified_tuples:
            tmp_data["x"], tmp_data["y"] = zip(*unified_tuples)
            tmp_data["x"] = list(tmp_data["x"])
            tmp_data["y"] = list(tmp_data["y"])

            result += [tmp_data]

    return result


def get_upload_metadata():
    if len(sys.argv) == 3:
        return sys.argv[1], sys.argv[2]

    print(
        "Wrong script run, usage: USER=user API_KEY=key plotly_upload_results.py BUILD_NUMBER benchmark_results.json"
    )
    return None, None


def get_new_benchmark_data(filename):
    # get benchmark results
    with open(filename) as data_file:
        return json.load(data_file)


def get_transformed_benchmark_data(filename, build_nr):
    try:
        benchmark_data = get_new_benchmark_data(filename)
    except (FileNotFoundError, ValueError):
        print('Benchmark data file not found or is not valid JSON.')
        return []

    return [
        dict(
            name=benchmark["name"],
            x=build_nr,
            y=benchmark["real_time"]  # TODO: Warning: not checking benchmark["time_unit"], assuming nanoseconds !!!
        ) for benchmark in benchmark_data['benchmarks']
    ]


def try_to_sign_in():
    usr = os.environ.get('USER')
    key = os.environ.get('API_KEY')
    if usr is None or key is None:
        print("Environment variables USER or API_KEY are missing!")
    py.sign_in(usr, key)


def send_data(new_data):
    try:
        # get plot_url or create new plot
        dummy_fig = {'data': [{'type': 'scatter', 'x': [], 'y': []}]}
        plot_url = py.plot(dummy_fig, filename=PLOTLY_FILENAME, fileopt='append', auto_open=False)

        # get plotly figure
        fig = py.get_figure(plot_url)

        # reset figure layout
        layout = go.Layout(
            title="IFJ Benchmark",
            titlefont=dict(size=20),
            xaxis=dict(
                title='<b>build #</b>',
                tickprefix='#',
                titlefont=dict(size=20),
                type='category',
                categoryorder='array',
                showticklabels=True,
                autorange=True
            ),
            yaxis=dict(
                title='time [ns]',
                ticksuffix='ns',
                titlefont=dict(size=20),
                showticklabels=True,
                autorange=True
            )
        )

        # combine with new data
        fig["data"] = plotly_combine(fig["data"], new_data)
        fig["layout"] = layout

        return py.plot(fig, filename=PLOTLY_FILENAME, fileopt="overwrite", auto_open=False)

    except Exception as e:
        print("Warning: benchmark results upload failed: ", e)


# ini
build_nr, benchmark_filename = get_upload_metadata()
data = get_transformed_benchmark_data(benchmark_filename, build_nr)  # login

try:
    try_to_sign_in()
except PlotlyError as e:
    print('Failed to sign in: ', e)
else:
    url = send_data(data)
    print("Benchmark results upload complete. ({0})".format(url))
