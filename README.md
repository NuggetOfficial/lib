# c
The current state of the repo is some code snippets I managed to create recently
combined into a single signal-driven hot-reloadable application-loop. The current 
implementation has a an atomic ring buffer with a depth of 16 tasks. It supports
growing signal amount (currently 3) and currently hard codes up to 8 attached
callbacks to every signal. It does all of this snuggely in 512 bytes. 
Still too much imo.

# todo
- [ ] consider relative pointers to cut memory requirement easily in 2 and maybe by 4
- [ ] consider demand-driven vtable allocation so that signals grow on attach for a
      more dynamic callback distribution
- [ ] consider 4-hastrie for enabling constant time callback detachment (in stead of
      current bulk detach)
