import os
import sys

venv_path = os.path.abspath(__file__ + '/../venv/lib/python2.7/site-packages')
sys.path.append(venv_path)

import PIL.Image

def get_texture(*args):
    # kwargs = dict(args[i:i + 2] for i in xrange(0, len(args), 2))
    image = PIL.Image.open('textures/mike2.jpg')
    image = image.convert("RGB")
    return image.size + (3, image.tostring())

