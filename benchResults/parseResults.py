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
openclCompTimes = {}

files = [f for f in os.listdir(".") if ".json" in f]
files.sort()
for filename in files:
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

        if implementation not in openclCompTimes:
            openclCompTimes[implementation] = {}
        if filename[:-5] not in openclCompTimes[implementation]:
            openclCompTimes[implementation][filename[:-5]] = []
        openclCompTimes[implementation][filename[:-5]].append(
            (settings["numberOfBodies"], benchmark["stepDuration"] / 10 ** 9))

    for val in graphData.values():
        val.sort()
    implementationCompTimes.append(graphData)

for g in implementationCompTimes:
    print(g)

for key, val in openclCompTimes.items():
    print(key, val)

from PyGnuplot import c, figure


def s(data, file=None):
    with open(file if file else "tmp.dat", "w") as f:
        for tuple_ in data:
            print(("{} " * len(tuple_)).rstrip().format(*tuple_), file=f)
            print(("{} " * len(tuple_)).rstrip().format(*tuple_))
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
        c(f'set style line {i + 1} lt 1 pt 7 lc rgb \'{color}\' lw 1.5 pi -1 ps 1.5')
    c("set pointintervalbox 2.5")


for name, val in implementationCompTimes[0].items():
    s(val, f"{name}.temp")

figure()
setLineStyles()
c(f"set key left")

plots = ""
for i, name in enumerate(implementationCompTimes[0].keys()):
    plots += f'"{name}.temp" using 1:2 title "{name}" with linespoints ls {i + 1},'
plots.rstrip(",")

c(f'plot {plots}')

figure()
setLineStyles()
c(f"set key left")




# for filename in [x for x in os.listdir(".") if ".temp" in x]:
#     os.remove(filename)
