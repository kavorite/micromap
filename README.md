# micromap

Sometimes, _you just want a hashmap._ This is the simplest dictionary
implementation that I'm aware could have been conceived. It uses an
open-addressing scheme with linear probing, and returns an error on insertions
or reallocations when the load factor exceeds 70%. It's designed to be easily
audited, easily reused, and easily read. Use as you wish. Or write a better
one. No skin off my back. For example usage, see [test.c][test.c].

[test.c]: https://github.com/kavorite/micromap/blob/master/test.c
