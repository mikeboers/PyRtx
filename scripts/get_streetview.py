'''

Assemble with:
    
    txmake -fov 120 -envcube test.*.jpg test-cube.tif


https://maps.google.com/maps?q=charles+and+commercial,+vancouver&ll=&spn=0.005439,0.009205&sll=49.273059,-123.069550&layer=c&cbp=13,98.04,,0,52.65&cbll=49.273059,-123.06955&hnear=Commercial+Dr&t=m&z=17&iwloc=A&panoid=JdRkcJKofLldx1HOMYj0Qw

'''

import os
import sys
from subprocess import call

for i, (name, heading, pitch) in enumerate([
    ('px', 0, 0),
    ('nx', 180, 0),
    ('py', 0, 90),
    ('py', 0, -90),
    ('pz', 90, 0),
    ('nz', 270, 0),
]):

    file_name = sys.argv[1] + '.%d.%s.jpg' % (i + 1, name)
    if not os.path.exists(file_name):
        url = (
            'http://maps.googleapis.com/maps/api/streetview?'
            'size=800x800&'
            'location=49.272642,-123.068901'
            '&fov=120&'
            'heading=%s&pitch=%s&sensor=false' % (heading, pitch)
        )
        call(['curl', '-o', file_name, url])
