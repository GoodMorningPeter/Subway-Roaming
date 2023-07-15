def count_strings_in_lines(input_file, output_file):
    with open(input_file, 'r', encoding='utf-8') as infile:
        lines = infile.readlines()

    counts = []
    for line in lines:
        strings = line.strip().split(' ')
        count = len(strings)
        counts.append(count)

    with open(output_file, 'w', encoding='gbk') as outfile:
        for count in counts:
            outfile.write(str(count) + '\n')


# 使用方法示例：
input_file = 'lines.txt'
output_file = 'output.txt'
count_strings_in_lines(input_file, output_file)