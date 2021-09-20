import textwrap

template = '''
template <{0}>
struct {name}<{1}> {{
\tusing F = {1};
\ttemplate <F func>
\tstatic auto __fastcall wrap({2}) {{
{3}
\t}}
}};
'''

template2 = '''
template <{0}>
struct {name}<{1}> {{
\t{2};
\tauto operator()({3}) {{
{4}
\t}}
}};
'''

'''
optcall:
this, a, b, c, d, args...
ecx, stack, stack, stack, stack, stack...
ecx, xmm1, xmm2, xmm3, stack...

optcall static:
a, b, c, d, args...
ecx, edx, stack, stack, stack...
xmm0, xmm1, xmm2, xmm3, stack...
'''

def gen(args, ret, thiscall=False):
    ts = []
    if not ret: ts.append('R')
    n = max(i if v else -1 for i, v in enumerate(args))
    special = False
    if thiscall and n == -1:
        n = 0
        special = True
    letter = lambda i: 'ABCD'[i]
    ts.extend([letter(i) for i, v in enumerate(args) if not v and i < n])
    if special: ts.append('A')
    ts.append('...Args')
    function_type = 'R(' + ', '.join(['float' if v else letter(i) for i, v in enumerate(args) if i <= n] + ['Args...']) + ')'
    function_args = [f'{letter(i)} a{i}' for i, v in enumerate(args) if not v and i < n] + ['Args... args']
    if special: function_args.insert(0, 'A a0')
    if thiscall:
        function_args.insert(1, 'void*')
    function_args = ', '.join(function_args)
    function_body = ''
    if any(args):
        function_body += 'float ' + ', '.join(f'a{i}' for i, v in enumerate(args) if v) + ';\n'
        function_body += '\n'.join(f'__asm movss a{i}, xmm{i}' for i, v in enumerate(args) if v) + '\n'
    function_body += 'return func(' + ', '.join([f'a{i}' for i, v in enumerate(args) if i <= n] + ['args...']) + ');'
    return template.format(
        ', '.join([f'typename {i}' for i in ts]),
        function_type,
        function_args,
        textwrap.indent(function_body, '\t\t'),
        name='optcall' if thiscall else 'optfastcall'
    )


def gen2(args, ret, thiscall=False):
    ts = []
    if not ret: ts.append('R')
    letter = lambda i: 'ABCD'[i]
    n = max(i if v else -1 for i, v in enumerate(args))
    ts.extend([letter(i) for i, v in enumerate(args) if not v and i < n])
    ts.append('...Args')
    function_type = 'R(' + ('__thiscall' if thiscall else '__fastcall') + '*)(' + \
        ', '.join(['float' if v else letter(i) for i, v in enumerate(args) if i <= n] + ['Args...']) + ')'
    function_type2 = 'R(' + ('__thiscall' if thiscall else '__fastcall') + '* addr)(' + \
        ', '.join([letter(i) for i, v in enumerate(args) if not v and i < n] + ['Args...']) + ')'
    function_args = ', '.join([f'{"float" if v else letter(i)} a{i}' for i, v in enumerate(args) if i <= n] + ['Args... args'])
    function_body = ''
    if any(args):
        function_body += '\n'.join(f'__asm movss xmm{i}, a{i}' for i, v in enumerate(args) if v) + '\n'
    function_body += 'return addr(' + ', '.join([f'a{i}' for i, v in enumerate(args) if not v and i < n] + ['args...']) +');'
    return template2.format(
        ', '.join([f'typename {i}' for i in ts]),
        function_type,
        function_type2,
        function_args,
        textwrap.indent(function_body, '\t\t'),
        name='WrapperOptcall' if thiscall else 'WrapperOptfastcall'
    )

for i in range(2**5):
    this, *args = [int(j) for j in f'{i:05b}']
    if this and args[0]: continue
    print(gen(args, False, thiscall=this))

for i in range(2**5):
    this, *args = [int(j) for j in f'{i:05b}']
    if this and args[0]: continue
    print(gen2(args, False, thiscall=this))
