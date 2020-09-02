import os
import shutil
import subprocess
import sys
import time

CLANG = 'c:/Programming/emsdk/upstream/bin/clang'
MAKE = 'mingw32-make'

srcpath = os.path.abspath(os.path.dirname(__file__))
projectpath = os.path.dirname(srcpath)

def log(*args):
    now = time.asctime()
    msg = '[{}] {}'.format(now, ' '.join((str(arg) for arg in args)))
    sys.stderr.flush()
    print(msg)
    sys.stdout.flush()

LAST_EDIT = {}
def has_changed(filename):
    cur = os.stat(filename).st_mtime
    built = LAST_EDIT.get(filename, 0)
    LAST_EDIT[filename] = cur
    return cur > built

def changed_in_dir(feef):
    ret = False
    for path, _, files in os.walk(feef):
        for name in files:
            base, ext = os.path.splitext(name)
            filename = os.path.join(path, name)
            if has_changed(filename):
                # don't early-return to update the cached time for all files
                ret = True
    return ret

def run_cmd(msg, cmd, cwd=None):
    log(msg, cmd)
    try:
        subprocess.check_call(cmd, cwd=cwd)
    except Exception as e:
        log('ERROR:', e)


########## program start

node = subprocess.Popen(['node', 'src/server.js'], cwd=projectpath)
try:
    while True:
        should_build = False
        if changed_in_dir('src'):
            should_build = True
        if changed_in_dir('../it-tools/src'):
            should_build = True
        if should_build:
            run_cmd('building', [MAKE, 'build'], cwd=projectpath)

        time.sleep(1.0)
finally:
    # make sure to clean up the node process handle
    node.terminate()
