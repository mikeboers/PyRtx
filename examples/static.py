import os
import sys

# Virtualenvs don't work in prman...
venv_path = os.path.abspath(__file__ + '/../../venv/lib/python2.7/site-packages')
sys.path.append(venv_path)

import PIL.Image

def rtx_open(**kwargs):
    image = PIL.Image.open('textures/van.jpg')
    image = image.resize((1024, 1024))
    image = image.convert("RGB")
    return image.size + (3, image.tostring())
