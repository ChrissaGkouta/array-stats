import subprocess
import re
import sys

# Πλήθος στοιχείων ανά πίνακα (Ν)
SIZES = [1000000, 10000000, 50000000] 
RUNS = 4
EXECUTABLE = "./array_stats"

def compile_code():
    print("--- Compiling Code ---")
    try:
        subprocess.run(["make"], check=True)
        print("Compilation successful.\n")
    except subprocess.CalledProcessError:
        print("Error: Compilation failed.")
        sys.exit(1)

def parse_time(output, label):
    # Regex για εντοπισμό δεκαδικού αριθμού μετά το label
    regex = rf"{label}:\s+([0-9\.]+)"
    match = re.search(regex, output)
    if match:
        return float(match.group(1))
    else:
        return None

def run_experiments():
    compile_code()

    print(f"{'N (Elements)':<15} | {'Avg Serial (s)':<15} | {'Avg Parallel (s)':<15} | {'Speedup':<10}")
    print("-" * 65)

    for n in SIZES:
        total_serial = 0.0
        total_parallel = 0.0
        
        for i in range(RUNS):
            # Κλήση του προγράμματος C: ./array_stats <N>
            result = subprocess.run([EXECUTABLE, str(n)], capture_output=True, text=True)
            
            if result.returncode != 0:
                print(f"Error running for N={n}")
                continue

            output = result.stdout
            
            # Ανάκτηση χρόνων από την έξοδο
            t_serial = parse_time(output, "Serial Execution Time")
            t_parallel = parse_time(output, "Parallel Execution Time")

            if t_serial is not None and t_parallel is not None:
                total_serial += t_serial
                total_parallel += t_parallel
            else:
                print(f"Warning: Could not parse times for N={n}, Run={i+1}")

        # Υπολογισμός Μέσων Όρων
        avg_serial = total_serial / RUNS
        avg_parallel = total_parallel / RUNS
        
        # Υπολογισμός Speedup 
        speedup = avg_serial / avg_parallel if avg_parallel > 0 else 0

        # Εκτύπωση αποτελεσμάτων για το συγκεκριμένο N
        print(f"{n:<15} | {avg_serial:<15.6f} | {avg_parallel:<15.6f} | {speedup:<10.2f}")

if __name__ == "__main__":
    run_experiments()