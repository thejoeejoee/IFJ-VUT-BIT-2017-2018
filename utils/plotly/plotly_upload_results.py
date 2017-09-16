#!/usr/bin/python

import os
import sys
import json
import plotly
import plotly.plotly as py
import plotly.graph_objs as go

#TODO: refactor needed
def parse(s):
    try:
        return int(s)
    except ValueError:
        return float(s)
def conv(v):
    if(isinstance(v,list)):
        return v
    elif(isinstance(v,int) or isinstance(v,float)):
        return [v]
    elif(isinstance(v,str)):
        return [parse(v)]
def plotly_combine(data_a, data_b):
    result = []
    # get set of names
    names = []
    for data in data_a:
        names += [data["name"]]
    for data in data_b:
        names += [data["name"]]
    names = set(names)

    # combine data
    for name in names:
        xy_tuples = []
        tmp_data = {    "type":"scatter",
                        "name":name, #name
                        "hoverinfo":"x+y+name",
                        "x":[], #build number
                        "y":[] #time
        }
        for data in data_a:
            if "name" in data.keys() and data["name"] == name:
                xy_tuples.extend(zip(conv(data["x"]),conv(data["y"])))
        for data in data_b:
            if "name" in data.keys() and data["name"] == name:
                xy_tuples.extend(zip(conv(data["x"]),conv(data["y"])))

        unified_tuples = list(dict(xy_tuples).items())
        unified_tuples.sort(key=lambda tup: tup[0])

        if(unified_tuples):
            tmp_data["x"], tmp_data["y"] = zip(*unified_tuples)
            tmp_data["x"] = list(tmp_data["x"])
            tmp_data["y"] = list(tmp_data["y"])

            result += [tmp_data]

    return result


# init
if (len(sys.argv) == 3):
    build_nr = sys.argv[1];
    filename = sys.argv[2];
else:
    print("usage: USER=user API_KEY=key plotly_upload_results.py BUILD_NUMBER benchmark_results.json")
    exit()

# get benchmark results
try:
    with open(filename,"r") as data_file:    
        benchmark_data = json.load(data_file)
except FileExistsError:
    print("Warning: benchmark file \"{0}\" not found".format(filename))
    exit()

new_data = []
for benchmark in benchmark_data["benchmarks"]:
    tmp_data = {    "name":benchmark["name"],
                    "x":build_nr,
                    "y":benchmark["time_ms"]
               }
    new_data += [tmp_data]

# login
usr = os.environ.get('USER')
key = os.environ.get('API_KEY')
if (usr==None or key==None):
    print("Environment variables USER or API_KEY are missing!")
    exit()

try:
    py.sign_in(usr, key)

    # get plot_url or create new plot
    dummy_fig = {'data': [{'type': 'scatter', 'x': [], 'y': []}]}
    plot_url = py.plot(dummy_fig, filename="IFJ_benchmark", fileopt='append', auto_open=False)

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
            title='time [ms]',
            ticksuffix='ms',
            titlefont=dict(size=20),
            showticklabels=True,
            autorange=True
        )
    )

    # combine with new data
    fig["data"] = plotly_combine(fig["data"], new_data)
    fig["layout"] = layout

    # upload

    result_url = py.plot(fig, filename="IFJ_benchmark", fileopt="overwrite", auto_open=False)
    print("Benchmark results upload complete. ({0})".format(result_url))
    exit()

except:
    print("Warning: benchmark results upload failed")
    exit()