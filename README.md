# GPU accelerated large number fixed point math using OpenCL
## TODOs:
- [x] Bitwise operations
- [x] Addition
- [ ] Signed numbers
- [x] Basic multiplication
- [ ] Improved multiplication (see Oancea & Watt)
- [ ] Exponentiation (integer exponents)
- [ ] Modulo
- [ ] Reduction operation
- [ ] Strassen-Schönhagen multiplication (if it's worth it?)

- [ ] Performance evaluation against GMP with openMP
- [ ] Performance evaluation against CGBN (nvidia)

- [x] OpenCL kernel operation scheduling CFGy using by finding data dependencies
- [ ] Kernel fusion / inst combine techniques
- [ ] Asynchronous batched memory transfers

# Notes

https://arxiv.org/html/2405.14642v1
https://github.com/NVlabs/CGBN
https://en.wikipedia.org/wiki/Montgomery_modular_multiplication
