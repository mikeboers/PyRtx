import json
import os
import sys
import random
from cStringIO import StringIO

venv_path = os.path.abspath(__file__ + '/../../venv/lib/python2.7/site-packages')
sys.path.append(venv_path)

import PIL.Image
import requests

urls = []

def rtx_open(**kwargs):

    if not urls:
        print 'Fetching Flickr feed...'
        raw_feed = requests.get('http://www.flickr.com/services/feeds/photos_public.gne?id=24585471@N05&format=json&nojsoncallback=1')
        try:
            encoded = raw_feed.text.replace(r'\/', '/').replace("\\'", "'")
            feed = json.loads(encoded)
        except:
            print encoded
            raise
        urls.extend(photo['media']['m'] for photo in feed['items'])
        random.shuffle(urls)

    url = urls.pop()
    print 'Fetching', url
    res = requests.get(url)
    image = PIL.Image.open(StringIO(res.content))
    image = image.resize((1024, 1024))
    image = image.convert("RGB")
    return image.size + (3, image.tostring())

