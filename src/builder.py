import os
import shutil
import subprocess
import sys
import time

CLANG = 'c:/Programming/emsdk/upstream/bin/clang'
MAKE = 'mingw32-make'

srcpath = os.path.abspath(os.path.dirname(__file__))
projectpath = os.path.dirname(srcpath)

print 'srcpath:',srcpath

def log(*args):
    now = time.asctime()
    msg = '[{}] {}'.format(now, ' '.join((str(arg) for arg in args)))
    sys.stderr.flush()
    print msg
    sys.stdout.flush()

LAST_EDIT = {}
def has_changed(filename):
    cur = os.stat(filename).st_mtime
    built = LAST_EDIT.get(filename, 0)
    LAST_EDIT[filename] = cur
    return cur > built

def run_cmd(msg, cmd, cwd=None):
    log(msg, cmd)
    try:
        subprocess.check_call(cmd, cwd=cwd)
    except Exception as e:
        log('ERROR:', e)

########## program start

node = subprocess.Popen(['node', '../src/server.js'], cwd=os.path.join(projectpath, 'out'))
try:
    while True:
        should_build = False
        for path, _, files in os.walk('src'):
            for name in files:
                base, ext = os.path.splitext(name)
                filename = os.path.join(path, name)
                if has_changed(filename):
                    should_build = True
        if should_build:
            run_cmd('building', [MAKE, 'build'], cwd=projectpath)
        time.sleep(0.5)
finally:
    # make sure to clean up the node process handle
    node.terminate()
