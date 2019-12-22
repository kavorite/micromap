# micromap

Sometimes, _you just want a hashmap._ This is the simplest dictionary
implementation that I'm aware could have been conceived, implemented in
portable, standard ISO C11. It uses an open-addressing scheme with linear
probing, and returns an error on insertions or reallocations when the load
factor exceeds 70%. It's designed to be easily audited, easily reused, and
easily read. Use as you wish. Or write a better one. No skin off my back. For
example usage, see [test.c][test.c], and feel free to copy and distribute the
source. 

Thanks to [@manuelVo][manuel] for reviewing the source, and
suggesting the changes made in [PR#1][pr1], and [this issue][delprobe].

[test.c]: https://github.com/kavorite/micromap/blob/master/test.c
[manuel]: https://github.com/manuelVo
[pr1]: https://github.com/kavorite/micromap/pull/1
[delprobe]: https://github.com/kavorite/micromap/issues/2
