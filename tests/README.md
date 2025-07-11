# Test Cases

This directory contains test cases for the pastry management system.

## Public Test Cases

The `public/` directory contains the official test cases provided for the project:

- **Input files**: `*.txt` - Test input commands
- **Expected output files**: `*.output.txt` - Expected program output

## Running Tests

To run a specific test case:

```bash
# Compile the program first
make

# Run a test case
./pasticceria < tests/public/example.txt

# Compare with expected output
./pasticceria < tests/public/example.txt > actual_output.txt
diff tests/public/example.output.txt actual_output.txt
```

## Test Cases Available

- `example.*` - Basic example test case
- `open1.*` to `open11.*` - Comprehensive test cases covering various scenarios