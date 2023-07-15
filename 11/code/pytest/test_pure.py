import subprocess, os, time, random
from tqdm import trange
from matplotlib import pyplot as plt

os.chdir("../")
command =  ['./fmain /b2 魏公村 良乡大学城北 8:30']
command += ['./fmain /b2 魏公村 良乡大学城北 12:30']
command += ['./fmain /b2 魏公村 良乡大学城北 18:30']
commandX =  ['./fmain /b2 魏X公村 良乡大学城北 8:30']
commandX += ['./fmain /b2 魏X公村 良乡大学城北 12:30']
commandX += ['./fmain /b2 魏X公村 良乡大学城北 18:30']
#cache warmup
for i in trange(10) : output = subprocess.check_output(command[0])
t, tX, tr, l = [], [], [], 100
for i in trange(l) :
    def test(cmd) :
        acommand = cmd[random.randint(0, len(cmd) - 1)]
        t0 = time.time()
        output = subprocess.check_output(acommand)
        return (time.time() - t0)*1000
    t.append(test(command))
    tX.append(test(commandX))
    tr.append(t[-1] - tX[-1])

plt.title("BIT subway roaming platform speed test")
plt.plot([_ for _ in range(l)], t, "b", label="all")
plt.plot([_ for _ in range(l)], tX, "r", label="building")
plt.plot([_ for _ in range(l)], tr, "g", label="calculation")
avg = sum(t)/len(t)
plt.plot([0, l-1], [avg, avg], "b")
print("avg:", avg)
avg = sum(tX)/len(tX)
plt.plot([0, l-1], [avg, avg], "r")
print("avg:", avg)
avg = sum(tr)/len(tr)
plt.plot([0, l-1], [avg, avg], "g")
print("avg:", avg)
plt.ylabel("ms")
plt.legend()
plt.show()
# print("avg:", sum(t)/len(t))
