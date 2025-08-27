#!/usr/bin/env python3
"""
Plot (n,k) combinations with highlighting of current parameter ranges
"""

import matplotlib.pyplot as plt
import re
import numpy as np

def parse_ans_file(filename):
    """Parse the ans.md file to extract (n,k) combinations for each d value"""
    combinations = {}
    
    try:
        with open(filename, 'r') as f:
            content = f.read()
        
        # Split by d=3 and d=4 sections
        sections = re.split(r'=== All valid \(n,k\) combinations for d = (\d+) ===', content)
        
        current_d = None
        for i, section in enumerate(sections):
            if section.strip() in ['3', '4']:
                current_d = int(section.strip())
                combinations[current_d] = []
            elif current_d is not None and i > 0:
                # Parse the table data
                lines = section.split('\n')
                for line in lines:
                    line = line.strip()
                    if line and not line.startswith('n\t') and not line.startswith('---') and not line.startswith('Total:'):
                        # Split by tab or multiple spaces
                        parts = re.split(r'\s+', line)
                        if len(parts) >= 2 and parts[0].isdigit() and parts[1].isdigit():
                            n, k = int(parts[0]), int(parts[1])
                            combinations[current_d].append((n, k))
        
        return combinations
    
    except FileNotFoundError:
        print(f"Error: Could not find file {filename}")
        return {}
    except Exception as e:
        print(f"Error parsing file: {e}")
        return {}

def plot_with_highlights(combinations):
    """Create plot highlighting current parameter ranges"""
    
    plt.figure(figsize=(14, 10))
    
    # Colors for different d values
    colors = {3: 'lightblue', 4: 'lightcoral'}
    highlight_colors = {3: 'darkblue', 4: 'darkred'}
    markers = {3: 'o', 4: 's'}
    
    # Current parameter ranges from your code
    current_n_range = [16, 17]
    current_k_range = [6, 7]
    current_d_range = [3, 4]
    
    # Plot all combinations
    for d in [3, 4]:
        if d in combinations and combinations[d]:
            n_vals = [pair[0] for pair in combinations[d]]
            k_vals = [pair[1] for pair in combinations[d]]
            
            plt.scatter(n_vals, k_vals, 
                       c=colors[d], 
                       marker=markers[d], 
                       alpha=0.4, 
                       s=20,
                       label=f'd = {d} (all valid)')
    
    # Highlight current ranges
    for d in current_d_range:
        if d in combinations:
            # Find combinations within current ranges
            highlighted = []
            for n, k in combinations[d]:
                if (min(current_n_range) <= n <= max(current_n_range) and 
                    min(current_k_range) <= k <= max(current_k_range)):
                    highlighted.append((n, k))
            
            if highlighted:
                h_n_vals = [pair[0] for pair in highlighted]
                h_k_vals = [pair[1] for pair in highlighted]
                
                plt.scatter(h_n_vals, h_k_vals, 
                           c=highlight_colors[d], 
                           marker=markers[d], 
                           alpha=0.9, 
                           s=100,
                           edgecolors='black',
                           linewidth=2,
                           label=f'd = {d} (current range: {len(highlighted)} combinations)')
    
    # Add rectangle to show current parameter space
    from matplotlib.patches import Rectangle
    rect = Rectangle((min(current_n_range) - 0.4, min(current_k_range) - 0.4), 
                    max(current_n_range) - min(current_n_range) + 0.8,
                    max(current_k_range) - min(current_k_range) + 0.8,
                    linewidth=3, edgecolor='black', facecolor='none', linestyle='--',
                    label='Current parameter space')
    plt.gca().add_patch(rect)
    
    # Add boundary lines
    n_range = np.linspace(1, 25, 100)
    plt.plot(n_range, n_range - 2, 'b--', alpha=0.7, linewidth=1, label='d=3 boundary: k = n - 2')
    plt.plot(n_range, n_range - 3, 'r--', alpha=0.7, linewidth=1, label='d=4 boundary: k = n - 3')
    
    plt.xlabel('n (codeword length)', fontsize=12)
    plt.ylabel('k (message length)', fontsize=12)
    plt.title('Valid (n,k) Combinations with Current Parameter Ranges Highlighted\n' + 
              f'Current ranges: n ∈ [{min(current_n_range)}, {max(current_n_range)}], ' +
              f'k ∈ [{min(current_k_range)}, {max(current_k_range)}], ' +
              f'd ∈ [{min(current_d_range)}, {max(current_d_range)}]', fontsize=14)
    plt.legend(fontsize=10, loc='upper left')
    plt.grid(True, alpha=0.3)
    plt.xlim(0, 26)
    plt.ylim(0, 21)
    
    plt.tight_layout()
    return plt

def main():
    # Parse the data from ans.md
    print("Parsing ans.md file...")
    combinations = parse_ans_file('ans.md')
    
    if not combinations:
        print("No data found or error parsing file.")
        return
    
    # Print summary of current parameter ranges
    current_n_range = [16, 17]
    current_k_range = [6, 7]
    current_d_range = [3, 4]
    
    print(f"\nCurrent parameter ranges from your code:")
    print(f"n ∈ [{min(current_n_range)}, {max(current_n_range)}]")
    print(f"k ∈ [{min(current_k_range)}, {max(current_k_range)}]")
    print(f"d ∈ [{min(current_d_range)}, {max(current_d_range)}]")
    
    print(f"\nValid combinations in your current ranges:")
    total_current = 0
    for d in current_d_range:
        if d in combinations:
            count = 0
            valid_combinations = []
            for n, k in combinations[d]:
                if (min(current_n_range) <= n <= max(current_n_range) and 
                    min(current_k_range) <= k <= max(current_k_range)):
                    count += 1
                    total_current += 1
                    valid_combinations.append((n, k))
            print(f"d = {d}: {count} combinations")
            for n, k in valid_combinations:
                print(f"  (n={n}, k={k})")
    
    print(f"\nTotal valid combinations in your current ranges: {total_current}")
    
    # Create highlighted plot
    print("\nCreating highlighted plot...")
    plt_highlight = plot_with_highlights(combinations)
    plt_highlight.savefig('nk_combinations_highlighted.png', dpi=300, bbox_inches='tight')
    print("Saved: nk_combinations_highlighted.png")
    
    # Show plot
    plt.show()
    
    print("\nHighlighted plot created!")
    print("This shows your current parameter ranges in dark colors with black borders.")

if __name__ == "__main__":
    main()
