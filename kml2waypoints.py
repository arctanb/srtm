import sys

if len(sys.argv) < 2:
  print 'Usage: %s <file containing path string>' % sys.argv[0]
  exit(0)

coord_str = ''
with open(sys.argv[1]) as fin:
  coord_str = fin.readline()
  fin.close()

coords = [[float(num) for num in coord.split(',')] for coord in
    coord_str.split(' ')]

print len(coords)

for coord in coords:
  if coord[1] < 0: sys.stdout.write('S ')
  else: sys.stdout.write('N ')
  a = int(coord[1])
  b = int((coord[1] - a) * 60)
  c = int((coord[1] - a - float(b)/60) * 60*60)
  sys.stdout.write(str(abs(a)) + ' ')
  sys.stdout.write(str(abs(b)) + ' ')
  sys.stdout.write(str(abs(c)) + ' ')
  if coord[0] < 0: sys.stdout.write('W ')
  else: sys.stdout.write('E ')
  a = int(coord[0])
  b = int((coord[0] - a) * 60)
  c = int((coord[0] - a - float(b)/60) * 60*60)
  sys.stdout.write(str(abs(a)) + ' ')
  sys.stdout.write(str(abs(b)) + ' ')
  sys.stdout.write(str(abs(c)))
  print ''
