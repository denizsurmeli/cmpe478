# Method
# No conccurency and parallelism during the benchmark.

# Compile all three programs.
# Run N times and take the average of all runs for each build.
#
# NOTES: The last float is the running time of the program.
import os
import subprocess

N = 100
thread_counts = [1, 2, 4, 8]
builds = ["serial", "parallel", "nolock"]
schedules = ["dynamic", "static", "guided"]
bounds = [100_000, 500_000, 1_000_000, 10_000_000]

"""
proc = subprocess.run(["./build/parallel",str(100_000)], capture_output=True, text=True)
print(proc.stdout.splitlines())
print(proc.returncode)
"""

for build in builds:
    for bound in bounds:
        if build != "serial":
            for thread_count in thread_counts:
                for schedule in schedules:
                    # perform the operation below N times
                   os.environ["OMP_SCHEDULE"] = schedule
                   os.environ["OMP_NUM_THREADS"] = str(thread_count)

                   proc = subprocess.run([f"./build/{build}",str(bound)], capture_output=True, text=True)
                   if proc.returncode != 0:
                       raise Exception("unexpected return code, returncode: ", proc.returncode)
                   
                   output = proc.stdout.splitlines()

                   elapsed = float(output[-1])

                   output = [int(x) for x in output[:-1]]
                   output.sort() 

                   print(build,schedule, output)


