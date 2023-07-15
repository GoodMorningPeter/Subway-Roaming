import subprocess, os, time, random
from matplotlib import pyplot as plt

os.chdir("../")
command =  ['./fmain /b2 魏公村 良乡大学城北 8:30']
command += ['./fmain /b2 魏公村 良乡大学城北 12:30']
command += ['./fmain /b2 魏公村 良乡大学城北 18:30']
command += ['./fmain /b2 九龙山 万寿路 6:12']
command += ['./fmain /b2 九龙山 万寿路 15:27']
command += ['./fmain /b2 纪家庙 望京东 19:55']
#cache warmup
for i in range(10) : output = subprocess.check_output(command[0])
t, l = [], 100
for i in range(l) :
    acommand = command[random.randint(0, len(command) - 1)]
    t0 = time.time()
    output = subprocess.check_output(acommand)
    t.append((time.time() - t0)*1000)

plt.title("BIT subway roaming platform speed test")
plt.plot([_ for _ in range(l)], t)
avg = sum(t)/len(t)
plt.plot([0, l-1], [avg, avg], "r")
plt.ylabel("ms")
plt.show()
print("avg:", sum(t)/len(t))

