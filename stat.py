import os
import re
import math

policy_list = ['LRU', 'LRU_2', 'CLOCK']
num_threads_list = [1, 2, 4, 8, 16, 32, 64]
metric_list = [
    'Hit count', 'Miss count', 'Hit rate', 'Read count', 'Write count',
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
