#!/usr/bin/python

import sys
import json
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



# get benchmark results

# benchmark results format example
"""
{
	"build":42,
	"benchmarks":[
		{"name":"func", "time_ms":10.42},
		{"name":"func_b", "time_ms":14.0},
		{"name":"func_c", "time_ms":16}
	]
}
"""

if (len(sys.argv) == 2):
    _filename = sys.argv[1];
else:
    print("usage: plotly_upload_results.py benchmark_results.json")
    exit()

with open(_filename,"r") as data_file:    
    benchmark_data = json.load(data_file)

build_nr = benchmark_data["build"]
new_data = []
for benchmark in benchmark_data["benchmarks"]:
    tmp_data = {    "name":benchmark["name"],
                    "x":build_nr,
                    "y":benchmark["time_ms"]
               }
    new_data += [tmp_data]


# get plotly figure
file = open("plot_url","r") 
plot_url = file.read()
file.close()
fig = py.get_figure(plot_url)

# combine with new data
fig["data"] = plotly_combine(fig["data"], new_data)

# upload
plot_url = py.plot(fig, filename="IFJ_benchmark", auto_open=False)