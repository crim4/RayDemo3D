#ifndef SOURCE_BASE_C
#define SOURCE_BASE_C

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define IS_IN_INCLUSIVE_RANGE(value, a, b) ((bool)IS_IN_RANGE(value, a, b + 1))
#define IS_IN_RANGE(value, a, b) ((bool)(value >= a && value < b))

// resizes a value from the original range to a new one
#define FROM_XRANGE_TO_YRANGE(value, xmin, xmax, ymin, ymax) \
    ((value - xmin) / (xmax - xmin) * ymax + ymin)

#endif