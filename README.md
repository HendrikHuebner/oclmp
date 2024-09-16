# GPU accelerated large number fixed point math
## TODOs:
- [x] Bitwise operations (trivial)
- [x] Addition using carry save 'trick'
- [ ] Signed numbers
- [ ] Basic multiplication
- [ ] Exponentiation (integer exponents)
- [ ] Modular multiplication (montgomery algorithm?)
- [ ] Strassen-Schönhagen multiplication (if it's worth it?)

- [ ] Performance evaluation against GMP with openMP
- [ ] Performance evaluation against CGBN (nvidia)

- [ ] Automatic operation scheduling using by finding data dependencies

# Notes

https://arxiv.org/html/2405.14642v1
https://github.com/NVlabs/CGBN
https://en.wikipedia.org/wiki/Montgomery_modular_multiplication
