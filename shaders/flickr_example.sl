surface flickr_example(string seed = "") {
    uniform string url = concat("rtxplugin:PythonRtx?module=examples.flickr&seed=", seed);
    Oi = Os;
    Ci = Cs * Oi * texture(url, 1 - s, 1 - t);
}
