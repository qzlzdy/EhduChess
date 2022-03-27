import subprocess

engine = subprocess.Popen(
    './stockfish',
    stdin=subprocess.PIPE,
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE,
    shell=True,
    text=True
)

outs = engine.stdout.readline() # Stockfish {version}
print(outs[:-1])

engine.stdin.write('uci\n')
engine.stdin.flush()
while True:
    outs = engine.stdout.readline()
    if outs == 'uciok\n':
        break
print(outs[:-1])

engine.stdin.write('setoption name Threads value 4\n')
print('set Threads=4')
engine.stdin.write('setoption name UCI_AnalyseMode value true\n')
print('set UCI_AnalyseMode=true')
engine.stdin.write('isready\n')
engine.stdin.flush()
outs = engine.stdout.readline() # readyok
print(outs[:-1])

engine.stdin.write('ucinewgame\n')
# TODO TCP/IP
engine.stdin.write('position startpos moves\n')
engine.stdin.write('go depth 20\n')
engine.stdin.flush()
while True:
    outs = engine.stdout.readline()
    if outs[:8] == 'bestmove':
        break
print(outs[:-1])
