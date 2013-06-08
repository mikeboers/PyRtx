Python Rtx Plugin for PRMan
===========================

This plugin for PRMan allows you to source textures from Python at render time.


Running the Example
-------------------

~~~
$ virtualenv venv
$ . venv/bin/activate
$ pip install requests pil
$ make
$ make flickr
~~~

(place results here)


Usage
-----

Make sure that the ``PythonRtx.so`` is on the PRMan texture path. Then use one of the "texture" family of shadeops with a texture name like:

~~~
rtxplugin:PythonRtx?module=yourmodule&function=yourfunction
~~~

This will call the ``yourfunction`` object of the ``yourmodule`` module. That function is expected to return a tuple with 4 objects:

1. the width of the image;
2. the height of the image;
3. the number of channels in the image;
4. the raw image data as a string.


Caveats
-------

- This currently behaves very badly when multithreaded; use ``-t:1`` in your renders if there is more than once instance of this plugin being used. I am working on understanding how to grab the GIL in this scenario.
- Image dimensions **MUST** be a power of two.
- We can currently only supply uint8 data, even though PRMan will take floats.

