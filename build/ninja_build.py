from ninja_syntax import Writer
import fnmatch, os

sources = []
include = []

object_fns = []
source_fns = []

def split_ext(fn):
    return fn.split(".")[0]

for root, dirnames, filenames in os.walk('../src'):
    for filename in fnmatch.filter(filenames, '*.c'):
        sources.append(split_ext(os.path.join(root, filename)))

for root, dirnames, filenames in os.walk('../include'):
    for filename in fnmatch.filter(filenames, '*.h'):
        sources.append(split_ext(os.path.join(root, filename)))

for source in sources:
    object_fns.append(source+'.o')
    source_fns.append(source+'.c')

with open("build.ninja", "w") as buildfile:
    writer = Writer(buildfile)
    out = StringIO()

    writer.build('out', 'cc', 'in', variables={" ".join(source_fns)})
