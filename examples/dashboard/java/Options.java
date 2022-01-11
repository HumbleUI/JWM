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

    public String get(int idx) {
        return _options[idx % size()];
    }

    public String set(int idx) {
        _idx = idx % size();
        return get();
    }

    public String set(String value) {
        for (int i = 0; i < _options.length; ++i) {
            if (_options[i].equals(value))
                return set(i);
        }
        return null;
    }

    public int size() {
        return _options.length;
    }

    public String next() {
        _idx = (_idx + 1) % size();
        return get();
    }

    public String prev() {
        _idx = (_idx + _options.length - 1) % _options.length;
        return get();
    }
}