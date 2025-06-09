#!/bin/bash
# filepath: lmerge_md_files.sh

output_file="merged_document.md"
echo "" > "$output_file"

for file in $(ls -1 [0-9]*.md | sort); do
    # Skip the output file if it exists in the directory
    if [ "$file" != "$output_file" ]; then
        echo -e "*File: $file*\n" >> "$output_file"
        cat "$file" >> "$output_file"
        echo -e "\n\n" >> "$output_file"
    fi
done

echo "All markdown files have been merged into $output_file"
