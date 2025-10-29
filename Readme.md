# LinkedIn Queens Puzzle Solver 

## Build

```bash
make clean
make
```

### 1. **Continuous Region Generation and Testing**
```bash
./linkedin_queens_solution 8 --regions
./linkedin_queens_solution 11 --regions
./linkedin_queens_solution 27 --regions
```

### 2. **Interactive Region Editor**
```bash
./linkedin_queens_solution 8 --edit
```

### 3. **Finding number of possible solutions**
```bash
./linkedin_queens_solution 11 --all --regions
```

### 4. **Change MAX_N for board size limit**
In line 6, ```puzzle.h```<br>  

### 5. **Memory Considerations with Board Sizes**
The arrays in the Puzzle structure are statically allocated, so memory usage is:

```
Puzzle structure size ≈ (5 × MAX_N²) + (2 × MAX_N³) integers

MAX_N = 16:  ~130 KB per puzzle
MAX_N = 32:  ~1 MB per puzzle
MAX_N = 64:  ~8 MB per puzzle
MAX_N = 100: ~40 MB per puzzle
```
### 6. **Computing times vary based on the puzzles**
The backtracking algorithm's performance depends on:

1. **Region layout**: Some layouts prune the search space more effectively
2. **Queen placement order**: Which region is solved first affects pruning
3. **Luck**: Sometimes the first path tried works, sometimes not.

### 7. **Drawbacks**
Since the puzzles are generated randomly, there might be situations when the puzzle is unsolvable i.e there's no solution. 
