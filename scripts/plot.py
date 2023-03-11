import argparse
import plotly.offline as pyo
import plotly.graph_objs as go

parser = argparse.ArgumentParser(description='Plot data from a file')
parser.add_argument('file', metavar='file', type=str, help='the input file name')

args = parser.parse_args()

# Read the data from the file
with open(args.file, 'r') as f:
    lines = f.readlines()[0:]

# Create empty lists for the x, y1, y2, and y3 values
x = []
y1 = []
y2 = []
y3 = []

# Iterate over the lines and extract the values
for line in lines:
    row = line.strip().split(';')
    x.append(float(row[0]))
    y1.append(float(row[1]))
    y2.append(float(row[2]))
    y3.append(float(row[3]))

# Create the plot
trace1 = go.Scatter(x=x, y=y1, mode='lines', name='y1')
trace2 = go.Scatter(x=x, y=y2, mode='lines', name='y2')
trace3 = go.Scatter(x=x, y=y3, mode='lines', name='y3')
data = [trace1, trace2, trace3]
layout = go.Layout(title='My Plot', xaxis=dict(title='X'), yaxis=dict(title='Y'))
fig = go.Figure(data=data, layout=layout)
fig.show()
