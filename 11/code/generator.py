stations, n = {}, 0
lines, m = {}, 0

with open("../data/subway_info_less.txt")  as f :
    file = f.readlines()
    for text_str in file :
        text = text_str.strip().split(" ")
        if (text[0] == "&") :
            stations[text[1]] = text[2]
        elif (text[0] == "@") :
            lines[text[1]] = text[3:]

def get_minute(h, m) :
    return h*60+m

import random
with open("../data/subway_timetable_less.txt", "w") as f:
    keys = list(lines.keys())
    print(keys)
    for i in range(len(keys)) :
        def generate(line) :
            #all the train are start in [4:54, 5:24]
            start_time = get_minute(4, 54) + random.randint(0, 30)
            #all the gap between two station is [3,6]
            gap_station = [random.randint(3,6) for _ in range(len(line))]
            #all the last train are start before 23:10
            t, end_time = start_time, get_minute(23, 10)
            #skip random station(Rapid train only stop large station)
            #skip start station can means different start station
            skip_rate = 0.05
            #all the train end before 23:10
            while t < end_time and t + sum(gap_station) < get_minute(23,59) + get_minute(00, 21) :
                cur = t
                table_line = []
                for j in range(len(line)) :
                    if random.random() < skip_rate : 
                        #repid train: original gap time - 1
                        cur += gap_station[j] - 1
                        continue
                    station_id = line[j]
                    table_line.append(str(cur))
                    table_line.append(stations[station_id])
                    cur += gap_station[j]

                writer = keys[i] + " " + str(len(table_line)//2) + " " + str(" ".join(table_line)) + "\n"
                f.write(writer)
                print(writer)

                #all the gap between two train start is [5, 12]
                gap_train = random.randint(5, 12)
                t += gap_train
        line = lines[keys[i]]
        generate(line)
        line.reverse()
        generate(line)
