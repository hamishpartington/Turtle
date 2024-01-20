#!/bin/bash

# Set the path to the two repositories
repo1="../Results"
repo2="../Neill_Results"
LOG_FILE="comp_pdf_out.txt"

exec 3>&1 1>>${LOG_FILE} 2>&1

# Loop through all files in repo1
for file in "$repo1"/*.pdf
do
    # Check if the file exists in repo2
    if [ -f "$repo2/$(basename "$file")" ]
    then
        # Execute the command on the file
        printf "%s: " $file
        comparepdf -c=a -v=2 "$file" "$repo2/$(basename "$file")"
    fi
done

for file in "$repo1"/*.txt
do
    # Check if the file exists in repo2
    if [ -f "$repo2/$(basename "$file")" ]
    then
        # Execute the command on the file
        printf "%s: " $file
        diff "$file" "$repo2/$(basename "$file")"
    fi
done
