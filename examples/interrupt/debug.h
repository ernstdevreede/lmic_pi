#include <stdio.h>


#define debug_init() 
#define debug_str(s) (fprintf(stderr, "%s\n", s))
#define debug_val(s,v) (fprintf(stderr, "%s: %d\n", s, v))
#define debug_led(v) (fprintf(stderr, "led: %d\n", v))
