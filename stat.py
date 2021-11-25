import os
import re
import math
import numpy as np
import matplotlib.pyplot as plt

policy_list = ['LRU', 'LRU_2', 'CLOCK']
num_threads_list = [1, 2, 4, 8, 16, 32, 64]
metric_list = [
    'Hit rate', 'Hit count', 'Miss count', 'Read count', 'Write count',
    'Elapsed time'
]

all_data = {}
for replacement in policy_list:
    for num_threads in num_threads_list:
        filename = f'replacement_{replacement}_threads_{num_threads}'
        files = [x for x in os.listdir('./log') if x.startswith(filename)]
        data = []
        for file in files:
            with open(os.path.join('./log', file)) as f:
                text = f.read()
                data.append({
                    metrics: float(
                        re.search(f'{metrics}: ([\d\.]+)(?:\n|\t|s)',
                                  text).group(1))
                    for metrics in metric_list
                })

        if replacement not in all_data:
            all_data[replacement] = {}
        all_data[replacement][num_threads] = {
            k: sum([x[k] for x in data]) / len(data)
            for k in data[0].keys()
        }


def dict2table(d, k_fn=str, v_fn=None, corner_name=''):
    '''
    Convert a nested dict to markdown table
    '''
    if v_fn is None:

        def v_fn(x):
            # Precision for abs(x):
            # [0, 0.1)    6
            # [0.1-1)    5
            # [1-10)    4
            # [10-100)    3
            # [100-1000)    2
            # [1000,oo)    1
            precision = max(1, min(5 - math.ceil(math.log(abs(x), 10)), 6))
            return f'{x:.{precision}f}'

    def parse_header(d, depth=0):
        assert depth in [0, 1], 'Only 1d or 2d dicts allowed'
        if isinstance(list(d.values())[0], dict):
            header = parse_header(list(d.values())[0], depth=depth + 1)
            for v in d.values():
                assert header == parse_header(v, depth=depth + 1)
            return header
        else:
            return f"| {' | '.join([corner_name] * depth + list(map(k_fn, d.keys())))} |"

    def parse_segmentation(d):
        return ' --- '.join(['|'] * parse_header(d).count('|'))

    def parse_content(d, accumulated_keys=[]):
        if isinstance(list(d.values())[0], dict):
            contents = []
            for k, v in d.items():
                contents.extend(parse_content(v, accumulated_keys + [k_fn(k)]))
            return contents
        else:
            return [
                f"| {' | '.join(accumulated_keys + list(map(v_fn, d.values())))} |"
            ]

    lines = [parse_header(d), parse_segmentation(d), *parse_content(d), '\n']
    return '\n'.join(lines)


hit_rate_and_IO_stat = {
    replacement: {
        key: sum([x[key] for x in value.values()]) / len(value)
        for key in metric_list if key != 'Elapsed time'
    }
    for replacement, value in all_data.items()
}

time_stat = {
    replacement: {k: v['Elapsed time']
                  for k, v in value.items()}
    for replacement, value in all_data.items()
}

print('**Hit rate and IO**\n')
print(dict2table(hit_rate_and_IO_stat, corner_name='Metric'))
print('**Elapsed time (s)**\n')
print(dict2table(time_stat, corner_name='Num threads'))

fig, (ax1, ax2) = plt.subplots(2, 1)
# make a little extra space between the subplots
fig.subplots_adjust(hspace=0.5)

width = 0.15
keys = list(hit_rate_and_IO_stat.values())[0].keys()
x = np.arange(len(keys))
ax1_twin = ax1.twinx()

for i, (key, value) in enumerate(hit_rate_and_IO_stat.items()):
    bars = ax1.bar(x[:1] + (1 - len(hit_rate_and_IO_stat)) / 2 * width +
                   width * i, [value['Hit rate']],
                   width=width,
                   label=key)
    bars = ax1_twin.bar(x[1:] + (1 - len(hit_rate_and_IO_stat)) / 2 * width +
                        width * i,
                        [v for k, v in value.items() if k != 'Hit rate'],
                        width=width,
                        label=key)

ax1.set_xlabel('Metric', labelpad=8)
ax1.set_ylabel('Rate')
ax1.set_ylim(0, 1)
ax1_twin.set_ylabel('Count')
ax1.set_xticks(x)
ax1.set_xticklabels(keys)
ax1.legend(fontsize=8)

for key, value in time_stat.items():
    ax2.plot([str(x) for x in value.keys()], value.values(), label=key)

ax2.set_xlabel('Num threads')
ax2.set_ylabel('Elapsed time (s)')
ax2.legend(fontsize=8)

# plt.show()
plt.savefig('./log/stat.svg', bbox_inches='tight')
