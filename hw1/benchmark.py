# Method
# No conccurency and parallelism during the benchmark.

# Compile all three programs.
# Run N times and take the average of all runs for each build.
#
# NOTES: The last float is the running time of the program.
import os
import subprocess
import statistics
import json

N = 10
thread_counts = ["1", "2", "4", "8"]
chunk_sizes = ["1","2","4","8","16","32","64","128","256","512","1024","2048","4096","8192"]
builds = ["parallel", "nolock"]
schedules = ["dynamic", "static", "guided"]
bounds = [100_000, 500_000, 1_000_000,1_500_000,2_000_000]

results = {}
for build in builds + ["serial"]:
    results[build] = {}
    for thread_count in thread_counts:
        results[build][thread_count] = {}
        for schedule in schedules:
            results[build][thread_count][schedule] = {}
            for chunk_size in chunk_sizes:
                results[build][thread_count][schedule][chunk_size] = {}
                for bound in bounds:
                    results[build][thread_count][schedule][chunk_size][str(bound)] = {}

for bound in bounds:
    elapsed_times = []
    outputs = []
    for i in range(N):
        cmd = [f"./build/serial", str(bound)]

        proc = subprocess.run(cmd, capture_output=True, text=True)
        if proc.returncode != 0:
            raise Exception(
                f"unexpected return code, returncode: {cmd}, {proc.returncode}",
            )
        output = proc.stdout.splitlines()

        elapsed = float(output[-1])

        output = [int(x) for x in output[:-1]]
        output.sort()

        if i > 0 and output != outputs[i - 1]:
            print(output)
            print(outputs[i - 1])
            raise Exception("unconsistent output on generation")

        outputs.append(output)
        elapsed_times.append(elapsed)
    results["serial"]["1"]["static"]["1"][str(bound)] = {
        "output": output,
        "avg_elapsed": statistics.mean(elapsed_times),
        "stdev_elapsed": statistics.stdev(elapsed_times),
    }

    print(
        f"serial,1,static-{bound}: ",
        results["serial"]["1"]["static"]["1"][str(bound)]["avg_elapsed"],
        results["serial"]["1"]["static"]["1"][str(bound)]["stdev_elapsed"],
    )

for build in builds:
    for bound in bounds:
        for thread_count in thread_counts:
            for schedule in schedules:
                for chunk_size in chunk_sizes:
                    # perform the operation below N times
                    os.environ["OMP_SCHEDULE"] = f"{schedule},{chunk_size}"
                    os.environ["OMP_NUM_THREADS"] = str(thread_count)

                    elapsed_times = []
                    outputs = []
                    for i in range(N):
                        cmd = [f"./build/{build}", str(bound)]
                        proc = subprocess.run(cmd, capture_output=True, text=True)
                        if proc.returncode != 0:
                            raise Exception(
                                f"unexpected return code, returncode: {cmd}, {proc.returncode}",
                            )

                        output = proc.stdout.splitlines()

                        elapsed = float(output[-1])

                        output = [int(x) for x in output[:-1]]
                        output.sort()

                        if i > 0 and output != outputs[i - 1]:
                            raise Exception("unconsistent output on generation")

                        outputs.append(output)
                        elapsed_times.append(elapsed)
                    results[build][str(thread_count)][schedule][str(chunk_size)][str(bound)] = {
                        "output": output,
                        "avg_elapsed": statistics.mean(elapsed_times),
                        "stdev_elapsed": statistics.stdev(elapsed_times),
                    }

                    print(
                        f"{build},{thread_count},{schedule}-{bound}: ",
                        results[build][str(thread_count)][schedule][str(chunk_size)][str(bound)][
                            "avg_elapsed"
                        ],
                        results[build][str(thread_count)][schedule][str(chunk_size)][str(bound)][
                            "stdev_elapsed"
                        ],
                    )

# intermediate step to save the runs in order to not run again
with open("results.json", "w") as f:
    json.dump(results, f)

header = ["build", "thread_count", "schedule", "chunk_size", "bound", "avg_elapsed", "stdev_elapsed"]
with open("results.csv", "w") as f:
    f.write(",".join(header) + "\n")
    for build in builds + ["serial"]:
        for thread_count in thread_counts:
            for schedule in schedules:
                for chunk_size in chunk_sizes:
                    for bound in bounds:
                        if (build == "serial" and chunk_size == "1" and schedule=="static" and thread_count=="1") or build != "serial": 
                            row = [build, thread_count, schedule, chunk_size, bound]
                            row.append(results[build][thread_count][schedule][chunk_size][str(bound)]["avg_elapsed"])
                            row.append(results[build][thread_count][schedule][chunk_size][str(bound)]["stdev_elapsed"])
                            f.write(",".join([str(x) for x in row]) + "\n")
