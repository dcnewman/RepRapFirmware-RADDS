import os.path

build_libraries = {

#  directory name Libraries/ : [
#    [ directories with include files, None=just the library directory itself],
#    [ directories to build, None=all ],
#    [ directories to ignore, None=none ],
#    [ arduino library dependencies ]
#  ]

  'EMAC' : [ None, None, None, ['core'] ],
  'Fatfs' : [ None, None, None, ['core'] ],
  'Flash' : [ None, None, None, ['core'] ],
  'I2C' : [ None, None, None, ['core'] ],
  'LCD_UI' : [ ['LCD_UI', 'I2C'], None, None, ['core'] ],
  'Lwip' : [ ['Lwip', 'Lwip/lwip/src/include'], None,
             ['Lwip/lwip/src/core/ipv6', 'Lwip/lwip/test'], [ ] ],
  'MAX31855' : [ ['MAX31855' ], None, None, ['core'] ],
  'MCP4461' : [ None, None, None, ['core'] ],
}


# Recursive descent listing of a directory tree

def list_dirs(dir, ignore=None):
  if ignore is None:
    ignore = [ ]
  list = []
  for d in dir:
    subdirs = [ d + '/' + name for name in os.listdir(d) if os.path.isdir(os.path.join(d, name)) and \
                    (name[0] != '.') and (not (d + '/' + name) in ignore) ]
    list += list_dirs(subdirs, ignore)
  return dir + list

def get_lib_dirs(lib):

  idirs = build_libraries[lib][2]
  if not idirs is None:
    idirs = [os.path.join('..', '..', 'src', 'Libraries', s) for s in idirs]
  else:
    idirs = [ ]

  if build_libraries[lib][1] == None:
    s = os.path.join('..', '..', 'src')
    tmp = list_dirs([os.path.join(s, 'Libraries', lib) ], idirs)
    subdirs = []
    for sd in tmp:
      subdirs.append(sd.lstrip(s))
  else:
    subdirs = []
    for d in build_libraries[lib][1]:
      subdirs.append(os.path.join('Libraries', d))

  subdirs.sort()

  if len(subdirs) == 0:
    subdirs = [ os.join('Libraries', lib) ]

  return subdirs

def append_path(paths, path):
  if not (path in paths):
    paths.append(path)

# Append to the list paths the strings
#
#     'Libraries/' + dirs[i]
#
# if dirs is None or '' then append just
#
#     'Libraries/' + dirname

def append_paths(paths, dirname, dirs = None):

  if (paths is None) or (dirname is None):
    return

  if (dirs is None) or (dirs == ''):
    append_path(paths, os.path.join('Libraries', dirname))
  elif type(dirs) is str:
    append_path(paths, os.path.join('Libraries', dirs))
  else:
    for d in dirs:
      append_path(paths, os.path.join('Libraries', d))

def append_arduino_path(lib, paths, base = None):
  if (lib is None) or (paths is None):
    return

  if base is None:
    base = ''

  if lib == 'core':
    append_path(paths, os.path.join(base, 'cores', 'arduino'))
    append_path(paths, os.path.join(base, 'system', 'libsam'))
    append_path(paths, os.path.join(base, 'system', 'CMSIS', 'Device', 'ATMEL'))
    append_path(paths, os.path.join(base, 'system', 'CMSIS', 'CMSIS', 'Include'))
    append_path(paths, os.path.join(base, 'variants', 'duet'))
    append_path(paths, os.path.join(base, 'system', 'libsam', 'include'))
    # append_path(paths, os.path.join(base, 'CMSIS', 'Device', 'ATMEL', 'sam3xa', 'include') )
  else:
    append_path(paths, os.path.join(base, 'libraries', lib))
