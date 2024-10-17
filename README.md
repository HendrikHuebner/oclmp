# GPU accelerated large number fixed point math using OpenCL
## TODOs:
- [x] Bitwise operations (trivial)
- [x] Addition using carry save 'trick'
- [ ] Signed numbers
- [x] Basic multiplication
- [ ] Exponentiation (integer exponents)
- [ ] Modulo
- [ ] Strassen-Schönhagen multiplication (if it's worth it?)

- [ ] Performance evaluation against GMP with openMP
- [ ] Performance evaluation against CGBN (nvidia)

- [x] Automatic operation scheduling using by finding data dependencies

# Notes

https://arxiv.org/html/2405.14642v1
https://github.com/NVlabs/CGBN
https://en.wikipedia.org/wiki/Montgomery_modular_multiplication
