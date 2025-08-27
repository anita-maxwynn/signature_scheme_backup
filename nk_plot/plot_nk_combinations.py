#!/usr/bin/env python3
"""
Plot (n,k) combinations for d=3 and d=4 from ans.md file
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

def plot_combinations(combinations):
    """Create scatter plots for the (n,k) combinations"""
    
    # Set up the plot
    plt.figure(figsize=(14, 10))
    
    # Colors for different d values
    colors = {3: 'blue', 4: 'red'}
    markers = {3: 'o', 4: 's'}
    
    # Plot each d value
    for d in [3, 4]:
        if d in combinations and combinations[d]:
            n_vals = [pair[0] for pair in combinations[d]]
            k_vals = [pair[1] for pair in combinations[d]]
            
            plt.scatter(n_vals, k_vals, 
                       c=colors[d], 
                       marker=markers[d], 
                       alpha=0.6, 
                       s=30,
                       label=f'd = {d} ({len(combinations[d])} points)')
    
    # Customize the plot
    plt.xlabel('n (codeword length)', fontsize=12)
    plt.ylabel('k (message length)', fontsize=12)
    plt.title('Valid (n,k) Combinations for Different Minimum Distances d\nUsing Singleton Bound: d ≤ n - k + 1', fontsize=14)
    plt.legend(fontsize=11)
    plt.grid(True, alpha=0.3)
    
    # Add the diagonal lines showing the Singleton bound
    n_max = max([max([pair[0] for pair in combinations[d]]) for d in combinations.keys() if combinations[d]])
    n_range = np.linspace(1, min(50, n_max), 100)
    
    # For d=3: k ≤ n - 2, so k_max = n - 2
    plt.plot(n_range, n_range - 2, 'b--', alpha=0.7, linewidth=1, label='d=3 boundary: k = n - 2')
    
    # For d=4: k ≤ n - 3, so k_max = n - 3  
    plt.plot(n_range, n_range - 3, 'r--', alpha=0.7, linewidth=1, label='d=4 boundary: k = n - 3')
    
    plt.legend(fontsize=10)
    
    # Set reasonable axis limits
    if combinations:
        all_n = []
        all_k = []
        for d in combinations:
            if combinations[d]:
                all_n.extend([pair[0] for pair in combinations[d]])
                all_k.extend([pair[1] for pair in combinations[d]])
        
        if all_n and all_k:
            plt.xlim(0, min(51, max(all_n) + 2))
            plt.ylim(0, max(all_k) + 2)
    
    plt.tight_layout()
    return plt

def create_zoomed_plot(combinations):
    """Create a zoomed-in plot for smaller n,k values"""
    plt.figure(figsize=(12, 8))
    
    colors = {3: 'blue', 4: 'red'}
    markers = {3: 'o', 4: 's'}
    
    # Plot only points where n <= 25 and k <= 20
    for d in [3, 4]:
        if d in combinations and combinations[d]:
            filtered_pairs = [(n, k) for n, k in combinations[d] if n <= 25 and k <= 20]
            
            if filtered_pairs:
                n_vals = [pair[0] for pair in filtered_pairs]
                k_vals = [pair[1] for pair in filtered_pairs]
                
                plt.scatter(n_vals, k_vals, 
                           c=colors[d], 
                           marker=markers[d], 
                           alpha=0.7, 
                           s=50,
                           label=f'd = {d} ({len(filtered_pairs)} points shown)')
    
    # Add boundary lines
    n_range = np.linspace(1, 25, 100)
    plt.plot(n_range, n_range - 2, 'b--', alpha=0.7, linewidth=1, label='d=3 boundary: k = n - 2')
    plt.plot(n_range, n_range - 3, 'r--', alpha=0.7, linewidth=1, label='d=4 boundary: k = n - 3')
    
    plt.xlabel('n (codeword length)', fontsize=12)
    plt.ylabel('k (message length)', fontsize=12)
    plt.title('Valid (n,k) Combinations - Zoomed View (n ≤ 25, k ≤ 20)', fontsize=14)
    plt.legend(fontsize=11)
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
    
    # Print summary
    for d in sorted(combinations.keys()):
        print(f"Found {len(combinations[d])} combinations for d = {d}")
    
    # Create full plot
    print("Creating full plot...")
    plt1 = plot_combinations(combinations)
    plt1.savefig('nk_combinations_full.png', dpi=300, bbox_inches='tight')
    print("Saved: nk_combinations_full.png")
    
    # Create zoomed plot
    print("Creating zoomed plot...")
    plt2 = create_zoomed_plot(combinations)
    plt2.savefig('nk_combinations_zoomed.png', dpi=300, bbox_inches='tight')
    print("Saved: nk_combinations_zoomed.png")
    
    # Show plots
    plt.show()
    
    print("\nPlots created successfully!")
    print("- nk_combinations_full.png: Shows all combinations")
    print("- nk_combinations_zoomed.png: Zoomed view for n ≤ 25, k ≤ 20")

if __name__ == "__main__":
    main()
