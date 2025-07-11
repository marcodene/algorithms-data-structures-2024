# Pastry Management System

**🏆 Final Grade: 30/30**

## 📋 Description
Software system for managing an industrial pastry that simulates the operation of orders, recipes, ingredients and shipments. The project was developed as a final exam for the Algorithms and Data Structures course at Politecnico di Milano.

## 🎯 Main Features
- **Recipe Management**: Addition and removal of recipes with related ingredients
- **Warehouse Management**: Ingredient restocking with batches and expiration dates
- **Order Management**: Order processing with ingredient availability check
- **Shipping System**: Periodic order loading on couriers with load optimization

## 📁 Project Structure

```
pastry-management-system/
├── README.md                 # Project documentation
├── Makefile                  # Build configuration
├── src/
│   └── main.c               # Main source code
├── tests/
│   ├── README.md            # Test documentation
│   └── public/              # Public test cases
│       ├── example.txt      # Example input
│       ├── example.output.txt # Expected output
│       └── open*.txt        # Additional test cases
├── docs/
│   ├── presentation/        # Project presentations
│   └── tools/              # Development tools and guides
│       ├── debugging-guide.pdf
│       ├── project-tools-guide.pdf
│       └── examples/       # Example code and configurations
└── archive/
    └── development-versions/ # Historical development versions
```

## 📦 Compilation

### Using Makefile (Recommended):
```bash
make              # Build the project
make debug        # Build with debug information
make asan         # Build with address sanitizer
make clean        # Clean build artifacts
```

### Manual compilation:
```bash
# Basic build
gcc -Wall -Werror -std=gnu11 -O2 -lm src/main.c -o pasticceria

# With debug information
gcc -Wall -Werror -std=gnu11 -O2 -g3 -lm src/main.c -o pasticceria
```

## 🚀 Execution

### Basic execution:
```bash
./pasticceria
```

### With input file:
```bash
./pasticceria < tests/public/example.txt
```

### With input and output files:
```bash
./pasticceria < tests/public/example.txt > output.txt
```

### Running tests:
```bash
make test                    # Run example test case
make test-all               # Run all public test cases
make test-file FILE=example # Test a specific file (without .txt extension)
make test-file FILE=open1   # Test open1.txt specifically
```

### Manual testing and output comparison:
```bash
# Run a single test manually
./pasticceria < tests/public/example.txt

# Save output and compare with expected result
./pasticceria < tests/public/example.txt > output.txt
diff tests/public/example.output.txt output.txt
```

## 📝 Input Format

The input file must start with the courier configuration:
```
<periodicity> <capacity>
```

Followed by commands (one per line):

### Available Commands

1. **aggiungi_ricetta**
   ```
   aggiungi_ricetta <recipe_name> <ingredient1> <quantity1> <ingredient2> <quantity2> ...
   ```
   Output: `aggiunta` or `ignorato`

2. **rimuovi_ricetta**
   ```
   rimuovi_ricetta <recipe_name>
   ```
   Output: `rimossa`, `ordini in sospeso` or `non presente`

3. **rifornimento**
   ```
   rifornimento <ingredient1> <quantity1> <expiration1> <ingredient2> <quantity2> <expiration2> ...
   ```
   Output: `rifornito`

4. **ordine**
   ```
   ordine <recipe_name> <number_of_items>
   ```
   Output: `accettato` or `rifiutato`

## 📊 Usage Example

### Sample input:
```
5 325
aggiungi_ricetta torta farina 50 uova 10 zucchero 20
aggiungi_ricetta ciambella farina 20 uova 5 burro 2
rifornimento farina 100 10 uova 100 10 zucchero 100 10 burro 100 10
ordine ciambella 6
ordine torta 1
```

### Expected output:
```
aggiunta
aggiunta
rifornito
camioncino vuoto
accettato
accettato
```

## 🐛 Debugging

### Using Makefile:
```bash
make debug        # Build with debug information
make asan         # Build with address sanitizer
make memcheck     # Run with valgrind memory checker
make profile      # Run with valgrind profiler
```

### Manual debugging:
```bash
# With GDB
make debug
gdb ./pasticceria

# With Address Sanitizer
make asan
./pasticceria < tests/public/example.txt

# With Valgrind (memory leaks)
valgrind --leak-check=full --show-leak-kinds=all ./pasticceria < tests/public/example.txt

# Profiling with Callgrind
valgrind --tool=callgrind ./pasticceria < tests/public/example.txt
kcachegrind callgrind.out.*
```

## 🏗️ Data Structures Used

- **Red-Black Trees**: For recipe book and pantry (efficient O(log n) search)
- **Min-Heap**: For batch management with priority on expiration
- **Linked Lists**: For ready/suspended orders and recipe ingredients

## ⚡ Implemented Optimizations

1. **Expiration Management**: Min-heap for quick access to batches closest to expiration
2. **Efficient Search**: RB trees for O(log n) search of recipes and foods
3. **Order Management**: Separate lists for ready and suspended orders
4. **Courier Loading**: Optimized algorithm to maximize load

## 📌 Important Notes

- Time is discrete, starts from 0 and increments after each command
- Expired batches are automatically removed
- Orders are prepared instantly if ingredients are available
- The courier arrives periodically (every n time instants)
- Orders are loaded by decreasing weight, with equal weight by chronological order

## 🚨 Limitations

- Recipe/ingredient names: max 255 characters, alphabet `[a-zA-Z_]`
- All numeric values must be positive integers (32 bit)
- No support for partial orders
- No multithreading
