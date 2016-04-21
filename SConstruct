import os
import sys
from os.path import join

sys.path.append('./scons_tools')
from platforms import *

# Load data from ~/.rrf_arduino_paths.py

tmp_dict = {}
home = expanduser('~')
if home[-1] != '/':
    home += '/'

site_file = home + '.rrf_arduino_paths.py'
if isfile(site_file):
    with open(site_file) as f:
        exec (f.read(), tmp_dict)
    keys_of_interest = ['bossac_path', 'coreduet_home', 'gccarm_bin']
    for key in keys_of_interest:
        if key in tmp_dict:
            if type(tmp_dict[key]) is str:
                exec (key + '="' + tmp_dict[key] + '"')
            elif type(tmp_dict[key]) is int:
                exec (key + '=' + str(tmp_dict[key]))
            else:
                raise Exception(key + ' in ' + site_file +
                                ' is of an unsupported type')

have_bossac = 'bossac_path' in globals()
have_home = 'coreduet_home' in globals()
have_bin = 'gccarm_bin' in globals()

if not (have_home and have_bin):
    raise Exception('You must first create the file ' + site_file + \
                    ' before building.  See ' + \
                    '~/sample_rrf_arduino_paths.py for an example.')

os.environ['COREDUET_HOME'] = coreduet_home
os.environ['GCCARM_BIN'] = gccarm_bin
if have_bossac:
    os.environ['BOSSAC_PATH'] = bossac_path
else:
    os.environ['BOSSAC_PATH'] = 'bossac'

platform = ARGUMENTS.get('platform', 'duet')
if not (platform in platforms):
    raise Exception('Platform ' + platform + ' is not currently supported')

os.environ['VARIANT_DIR'] = join('build', platform)

SConscript(['src/SConscript.due'], variant_dir=os.environ['VARIANT_DIR'])
