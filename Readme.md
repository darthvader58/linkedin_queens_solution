# LinkedIn Queens Puzzle Solver 

## Build

```bash
make clean
make
```

### 1. **Continuous Region Generation (Default)**
```bash
./linkedin_queens_solution 8 --regions
./linkedin_queens_solution 11 --regions
./linkedin_queens_solution 27 --regions
```

### 2. **Interactive Region Editor**
```bash
./linkedin_queens_solution 8 --edit
```

### 3. **Change MAX_N for board size limit**
In line 6, ```puzzle.h```<br>  
