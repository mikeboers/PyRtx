
def entrypoint(*args, **kwargs):
    print 'hello from the entrypoint:', args, kwargs
    raise ValueError('testing')

