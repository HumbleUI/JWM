package io.github.humbleui.jwm.examples;

public class Options {
    public final String[] _options;
    public int _idx;

    public Options(int idx, String... options) {
        _options = options;
        _idx = idx;
    }

    public Options(String... options) {
        this(0, options);
    }

    public String get() {
        return _options[_idx];
    }

    public String next() {
        _idx = (_idx + 1) % _options.length;
        return _options[_idx];
    }

    public String prev() {
        _idx = (_idx + _options.length - 1) % _options.length;
        return _options[_idx];
    }
}