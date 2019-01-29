import os
import json

# To compare:
# For each file: Compare different implementations (cpu-single-thread vs. cpu-multi-thread vs. opencl)
# For each tuple of settings: Compare different files.
# Interesting comparisons: time, speedup.
# x-axis: numBodies
# y-axis: time or speedup
# legend: different settings/implementations

implementationCompTimes = []
# openclCompTimes = {}

files = [f for f in os.listdir(".") if ".json" in f]
files = [f for f in files if "augmented.json" in f]
files.sort()
for filename in files:
    print("file:", filename)
    with open(filename, "r") as f:
        contents = json.loads(f.read())

    print(contents)
    graphData = {}
    for benchmark in contents["benchmarks"]:
        settings = benchmark["settings"]
        implementation = f"{settings['algorithm']}-{settings['platform']}-{settings['floatingPointType']}"
        # print(implementation)
        if implementation not in graphData:
            graphData[implementation] = []
        graphData[implementation].append((settings["numberOfBodies"], benchmark["stepDuration"] / 10 ** 9))

        # if implementation not in openclCompTimes:
        #     openclCompTimes[implementation] = {}
        # if filename[:-5] not in openclCompTimes[implementation]:
        #     openclCompTimes[implementation][filename[:-5]] = []
        # openclCompTimes[implementation][filename[:-5]].append(
        #     (settings["numberOfBodies"], benchmark["stepDuration"] / 10 ** 9))

    for val in graphData.values():
        val.sort()
    implementationCompTimes.append(graphData)

for g in implementationCompTimes:
    print(g)

# for key, val in openclCompTimes.items():
#     print(key, val)

from PyGnuplot import c, figure


def s(data, file=None):
    with open(file if file else "dat.temp", "w") as f:
        for tuple_ in data:
            print(("{} " * len(tuple_)).rstrip().format(*tuple_), file=f)
            # print(("{} " * len(tuple_)).rstrip().format(*tuple_))
            # f.write(f"{x} {y}\n")


def setLineStyles():
    colors = [
        "#396AB1",
        "#DA7C30",
        "#3E9651",
        "#CC2529",
        "#535154",
        "#6B4C9A",
        "#922428",
        "#948B3D",
    ]
    for i, color in enumerate(colors):
        # print(f'set style line {i + 1} lt 1 pt 7 lc rgb "{color}" lw 1.5')
        c(f'set style line {i + 1} lt 1 pt 7 lc rgb \'{color}\' lw 1.5 pi -0 ps 1.5')
        # c(f'set style line {i + 1} lt 1 pt 7 lc rgb \'{color}\' lw 1.5 pi -1 ps 1.5')
    c("set pointintervalbox 2.5")


def stripPrefix(name):
    return name.replace("brute-force", "").replace("barnes-hut", "").strip("-")


def stripSuffix(name):
    return name.replace("double", "").replace("float", "").strip("-")


def makeGraph(graphName, stripFPType, implementationFilter, xSettings, plotRange=None, speedUpBase=None):
    figure()
    c(f"set term epslatex")
    # c(f"set term wxt")
    setLineStyles()
    for line in xSettings.split("\n"):
        line = line.strip()
        if line:
            c(f"{line}")
    plots = ""
    for i, (name, val) in enumerate((name, val) for name, val in implementationCompTimes[0].items()
                                    if implementationFilter(name)):
        isDouble = "float" in name
        name = stripPrefix(name)
        if stripFPType:
            name = stripSuffix(name)
        if speedUpBase:
            baseTimes = implementationCompTimes[0][speedUpBase]
            s([(nbodies, baseTime/time) for (nbodies, time), (_, baseTime) in zip(val, baseTimes)], f"{graphName}{name}Speedup.temp")
            plots += f'"{graphName}{name}Speedup.temp" using 1:2 title "{name}" with linespoints ls {i + 1} {"pt 9" if isDouble else ""},'
        else:
            s(val, f"{graphName}{name}.temp")
            plots += f'"{graphName}{name}.temp" using 1:2 title "{name}" with linespoints ls {i + 1} {"pt 9" if isDouble else ""},'
    plots.rstrip(",")
    print(graphName)
    c(f'set output "{graphName}.tex"')
    plotRange = plotRange if plotRange is not None else ""
    c(f'plot {plotRange} {plots}')
    c(f'unset output')

'''
makeGraph(
    "compareAll",
    False,
    lambda name: True,
    """
    set logscale xy 2
    set grid xtics ytics
    set key outside above left
    set xlabel "\\\\\\#bodies
    set ylabel "Seconds"
    """
)
'''
makeGraph(
    "compareOpenCL",
    False,
    lambda name: "opencl" in name,
    """
    set logscale xy 2
    set grid xtics ytics
    set key above left
    set key font ",18"
    set xlabel "\\\\\\#bodies
    set ylabel "Seconds"
    """
)
makeGraph(
    "speedupOpenCL",
    False,
    lambda name: "opencl" in name,
    """
    set logscale xy 2
    set grid xtics ytics
    set key above left
    set key font ",18"
    set xlabel "\\\\\\#bodies
    set ylabel "Speedup"
    """,
    None,
    "brute-force-opencl-double"
)
makeGraph(
    "compareDifferentDouble",
    True,
    lambda name: "double" in name and ("cpu" in name or "opencl-" in name),
    """
    set logscale xy 2
    set grid xtics ytics
    set key above left
    set key font ",16"
    set xlabel "\\\\\\#bodies
    set ylabel "Seconds"
    """
)
makeGraph(
    "speedupDifferentDouble",
    True,
    lambda name: "double" in name and ("cpu" in name or "opencl-" in name),
    """
    set logscale xy 2
    set grid xtics ytics
    set key above left
    set key font ",18"
    set arrow from 128,6 to 32786,6 nohead ls 0
    set xlabel "\\\\\\#bodies
    set ylabel "Speedup"
    """,
    "[][0.5:*]",
    "brute-force-cpu-single-thread-double"
)
makeGraph(
    "compareCpu",
    False,
    lambda name: "cpu" in name,
    """
    set logscale xy 2
    set grid xtics ytics
    set key above left
    set xlabel "\\\\\\#bodies
    set ylabel "Seconds"
    """,
)

import time
time.sleep(1)
for filename in [x for x in os.listdir(".") if ".temp" in x]:
    os.remove(filename)
