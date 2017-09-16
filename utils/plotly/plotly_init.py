import plotly
import plotly.plotly as py
import plotly.graph_objs as go

if (input("Enter credentials? y/n:") == "y"):
    usr = input("Enter plot.ly username:")
    key = input("Enter plot.ly api_key:")
    plotly.tools.set_credentials_file(username=usr, api_key=key)

layout = go.Layout(
    title="IFJ Benchmark",
    titlefont=dict(size=20),
    xaxis=dict(
        title='<b>build #</b>',
        tickprefix='#',
        titlefont=dict(size=20), 
        type='category',
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

data = go.Data([
    go.Scatter(
        x=[],
        y=[]
    )
])

fig = go.Figure(data=data,layout=layout)

if (input("Create new file? y/n:") == "y"):
    plot_url = py.plot(fig, filename='IFJ_benchmark', fileopt='new', auto_open=False)
else:
    plot_url = py.plot(fig, filename='IFJ_benchmark', auto_open=False)

file = open("plot_url","w") 
file.write(plot_url);