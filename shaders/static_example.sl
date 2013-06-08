surface static_example() {
    Oi = Os;
    Ci = Cs * Oi * texture("rtxplugin:PythonRtx?module=examples.static", s, 1 - t);
}