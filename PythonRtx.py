import json
import os
import sys
from cStringIO import StringIO

venv_path = os.path.abspath(__file__ + '/../venv/lib/python2.7/site-packages')
sys.path.append(venv_path)

import PIL.Image
import requests

def get_texture(*args):

    print 'Getting list from Flickr...'
    #res = requests.get('http://api.flickr.com/services/feeds/photos_public.gne?format=json&nojsoncallback=1')
    #res = json.loads(res.text)

    #url = res['items'][0]['media'].values()[0]
    #url = 'http://farm9.staticflickr.com/8519/8546904513_2182e00432_c.jpg'
    #print 'Getting', url
    #res = requests.get(url)

    #image = PIL.Image.open(StringIO(res.content))
    #image.save('test.jpg')
    # kwargs = dict(args[i:i + 2] for i in xrange(0, len(args), 2))
    
    image = PIL.Image.open('textures/mike2.jpg')
    image = image.convert("RGB")
    return image.size + (3, image.tostring())

