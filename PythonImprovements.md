# Python Mandelbrot set generation speed improvements

Let's try iteratively improve current python solution.

## `gen_complex_set()`

### Default implementation (3 runs)

| ncalls | tottime | percall | cumtime | percall | filename:lineno(function)         |
| ------ | ------- | ------- | ------- | ------- | --------------------------------- |
| 1      | 3.276   | 3.276   | 3.281   | 3.281   | mandelbrot.py:18(gen_complex_set) |
| 1      | 3.444   | 3.444   | 3.450   | 3.450   | mandelbrot.py:18(gen_complex_set) |
| 1      | 3.321   | 3.321   | 3.326   | 3.326   | mandelbrot.py:18(gen_complex_set) |


| mean(ncalls) | mean(tottime) | mean(percall) | mean(cumtime) | mean(percall) | filename:lineno(function)         |
| ------------ | ------------- | ------------- | ------------- | ------------- | --------------------------------- |
| 1            | 3.347         | 3.347         | 3.352         | 3.352         | mandelbrot.py:18(gen_complex_set) |



### Move `* 1j` out of loop

| ncalls | tottime | percall | cumtime | percall | filename:lineno(function)        |
| ------ | ------- | ------- | ------- | ------- | -------------------------------- |
| 1      | 0.823   | 0.823   | 0.829   | 0.829   | mandelbrot.py:37(gen_complex_v1) |
| 1      | 0.747   | 0.747   | 0.752   | 0.752   | mandelbrot.py:37(gen_complex_v1) |
| 1      | 0.733   | 0.733   | 0.739   | 0.739   | mandelbrot.py:37(gen_complex_v1) |


| mean(ncalls) | mean(tottime) | mean(percall) | mean(cumtime) | mean(percall) | filename:lineno(function)        |
| ------------ | ------------- | ------------- | ------------- | ------------- | -------------------------------- |
| 1            | 0.768         | 0.768         | 0.773         | 0.773         | mandelbrot.py:18(gen_complex_v1) |


### Same as previous + use Cartesian product

| ncalls | tottime | percall | cumtime | percall | filename:lineno(function)        |
| ------ | ------- | ------- | ------- | ------- | -------------------------------- |
| 1      | 0.000   | 0.000   | 0.038   | 0.038   | mandelbrot.py:56(gen_complex_v2) |
| 1      | 0.000   | 0.000   | 0.030   | 0.030   | mandelbrot.py:56(gen_complex_v2) |
| 1      | 0.000   | 0.000   | 0.028   | 0.028   | mandelbrot.py:56(gen_complex_v2) |


| mean(ncalls) | mean(tottime) | mean(percall) | mean(cumtime) | mean(percall) | filename:lineno(function)        |
| ------------ | ------------- | ------------- | ------------- | ------------- | -------------------------------- |
| 1            | 0.000         | 0.000         | 0.032         | 0.032         | mandelbrot.py:18(gen_complex_v2) |

## `mandelbrot_sequence()`

### Default implementation (3 runs)

| ncalls | tottime | percall | cumtime | percall | filename:lineno(function)             |
| ------ | ------- | ------- | ------- | ------- | ------------------------------------- |
| 1      | 0.228   | 0.228   | 7.842   | 7.842   | mandelbrot.py:73(mandelbrot_sequence) |
| 1      | 0.219   | 0.219   | 7.495   | 7.495   | mandelbrot.py:73(mandelbrot_sequence) |
| 1      | 0.244   | 0.244   | 7.949   | 7.949   | mandelbrot.py:73(mandelbrot_sequence) |


| mean(ncalls) | mean(tottime) | mean(percall) | mean(cumtime) | mean(percall) | filename:lineno(function)             |
| ------------ | ------------- | ------------- | ------------- | ------------- | ------------------------------------- |
| 1            | 0.230         | 0.230         | 7.762         | 7.762         | mandelbrot.py:73(mandelbrot_sequence) |


### Move RGB2BGR conversion out of loop and function

| ncalls | tottime | percall | cumtime | percall | filename:lineno(function)                |
| ------ | ------- | ------- | ------- | ------- | ---------------------------------------- |
| 1      | 0.225   | 0.225   | 7.991   | 7.991   | mandelbrot.py:97(mandelbrot_sequence_v1) |
| 1      | 0.218   | 0.218   | 7.821   | 7.821   | mandelbrot.py:97(mandelbrot_sequence_v1) |
| 1      | 0.223   | 0.223   | 7.594   | 7.594   | mandelbrot.py:97(mandelbrot_sequence_v1) |


| mean(ncalls) | mean(tottime) | mean(percall) | mean(cumtime) | mean(percall) | filename:lineno(function)                |
| ------------ | ------------- | ------------- | ------------- | ------------- | ---------------------------------------- |
| 1            | 0.222         | 0.222         | 7.802         | 7.802         | mandelbrot.py:97(mandelbrot_sequence_v1) |


### Same as previous + replace `mandelbrot_vec()` with `mandelbrot()`

| ncalls | tottime | percall | cumtime | percall | filename:lineno(function)                 |
| ------ | ------- | ------- | ------- | ------- | ----------------------------------------- |
| 1      | 0.229   | 0.229   | 0.319   | 0.319   | mandelbrot.py:119(mandelbrot_sequence_v2) |
| 1      | 0.224   | 0.224   | 0.313   | 0.313   | mandelbrot.py:119(mandelbrot_sequence_v2) |
| 1      | 0.223   | 0.223   | 0.308   | 0.308   | mandelbrot.py:119(mandelbrot_sequence_v2) |


| mean(ncalls) | mean(tottime) | mean(percall) | mean(cumtime) | mean(percall) | filename:lineno(function)                 |
| ------------ | ------------- | ------------- | ------------- | ------------- | ----------------------------------------- |
| 1            | 0.225         | 0.225         | 0.313         | 0.313         | mandelbrot.py:119(mandelbrot_sequence_v2) |



## Results

### `gen_complex_set()`

| filename:lineno(function)         | mean(cumtime) | improvement, % |
| --------------------------------- | ------------- | -------------- |
| mandelbrot.py:18(gen_complex_set) | 3.352         | baseline       |
| mandelbrot.py:18(gen_complex_v1)  | 0.773         | 76.9           |
| mandelbrot.py:18(gen_complex_v2)  | 0.032         | 99.0           |


### `mandelbrot_sequence()`

| filename:lineno(function)                 | mean(cumtime) | improvement, % |
| ----------------------------------------- | ------------- | -------------- |
| mandelbrot.py:73(mandelbrot_sequence)     | 7.762         | baseline       |
| mandelbrot.py:97(mandelbrot_sequence_v1)  | 7.802         | -0.5           |
| mandelbrot.py:119(mandelbrot_sequence_v2) | 0.313         | 96.0           |

