# Default target
all: merge


.PHONY: all clean merge help


# Target to merge markdown files
merge:
	@echo "Merging markdown files..."
	@chmod +x merge_md_files.sh
	@./merge_md_files.sh

# Target to remove the merged file
clean:
	@echo "Cleaning up..."
	@rm -f merged_document.md
	@echo "Removed merged_document.md"

# Help information
help:
	@echo "Available targets:"
	@echo "  all     - Default target, runs 'merge'"
	@echo "  merge   - Merges markdown files into merged_document.md"
	@echo "  clean   - Removes merged_document.md"
	@echo "  help    - Shows this help information"