#!/bin/bash

# Define Git repository URLs and (optionally) branches
REPO_1_URL="git@github.com:git/git.git"
BRANCH_1="master"

REPO_2_URL="git@github.com:git/git.git"
BRANCH_2="main"

# Create temporary directories for cloning
TEMP_DIR_1=$(mktemp -d)
TEMP_DIR_2=$(mktemp -d)

# Output log file
LOGFILE="srcdiff_git_comparison.log"

# Function to track srcDiff's performance
track_srcdiff_performance() {
    COMMAND="$1"
    echo "Running: $COMMAND" >> $LOGFILE
    /usr/bin/time -v $COMMAND 2>> $LOGFILE &
    SRC_DIFF_PID=$!

    # Monitor srcDiff performance using pidstat
    echo "Tracking performance for srcDiff (PID: $SRC_DIFF_PID)" >> $LOGFILE
    pidstat -p $SRC_DIFF_PID 1 >> $LOGFILE &
    PIDSTAT_PID=$!

    # Wait for srcDiff to finish and then stop performance monitoring
    wait $SRC_DIFF_PID
    echo "Finished running srcDiff (PID: $SRC_DIFF_PID)" >> $LOGFILE
}

# Clone repositories
echo "Cloning repository $REPO_1_URL (branch $BRANCH_1) into $TEMP_DIR_1" >> $LOGFILE
git clone --branch "$BRANCH_1" "$REPO_1_URL" "$TEMP_DIR_1" || { echo "Error: Failed to clone $REPO_1_URL" >> $LOGFILE; exit 1; }

echo "Cloning repository $REPO_2_URL (branch $BRANCH_2) into $TEMP_DIR_2" >> $LOGFILE
git clone --branch "$BRANCH_2" "$REPO_2_URL" "$TEMP_DIR_2" || { echo "Error: Failed to clone $REPO_2_URL" >> $LOGFILE; exit 1; }

# Check if any .cpp or .h files are found
FILES_1=$(find "$TEMP_DIR_1" -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.c" \))
echo "Files found in $TEMP_DIR_1:"
echo "$FILES_1"

FILES_2=$(find "$TEMP_DIR_2" -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.c" \))
echo "Files found in $TEMP_DIR_2:"
echo "$FILES_2"

# Color codes for output messages
ERROR='\033[0;31m'        # Red for errors
SEQUENCE='\033[0;33m'     # Orange for sequential comparisons
PARALLEL='\033[0;34m'     # Green for parallel comparisons
NC='\033[0m'              # No color

# Original Sequential File Comparison
# -----------------------------------
echo -e "${SEQUENCE}Starting sequential comparison of individual files...${NC}" >> $LOGFILE

# Start timer for total sequential execution time
SEQUENTIAL_TOTAL_START_TIME=$SECONDS

# Find all .cpp and .h files in the first repository
find "$TEMP_DIR_1" -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.c" \) | while read -r FILE_1; do
    # Get the relative path of the file
    REL_PATH="${FILE_1#$TEMP_DIR_1/}"

    # Find the corresponding file in the second repository
    FILE_2="$TEMP_DIR_2/$REL_PATH"

    if [ -f "$FILE_2" ]; then
        # Increment sequential comparison count
        ((SEQUENTIAL_COMPARISON_COUNT++))

        # Print progress message to the console and log
        echo -e "${SEQUENCE}Comparing $(basename "$FILE_1") from $BRANCH_1 against $(basename "$FILE_2") from $BRANCH_2${NC}"
        echo -e "${SEQUENCE}Comparing $(basename "$FILE_1") from $BRANCH_1 against $(basename "$FILE_2") from $BRANCH_2${NC}" >> $LOGFILE

        # Run srcDiff and track its performance
        track_srcdiff_performance "srcdiff $FILE_1 $FILE_2 -o /dev/null"
    else
        # Log files that do not exist in the second repository in red
        echo -e "${ERROR}File $FILE_2 does not exist in $TEMP_DIR_2${NC}"
        echo "File $FILE_2 does not exist in $TEMP_DIR_2" >> $LOGFILE
    fi
done

SEQUENTIAL_TOTAL_END_TIME=$SECONDS
SEQUENTIAL_TOTAL_TIME=$((SEQUENTIAL_TOTAL_END_TIME - SEQUENTIAL_TOTAL_START_TIME))

# Find files in the second repository that do not exist in the first
find "$TEMP_DIR_2" -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.c" \) | while read -r FILE_2; do
    # Get the relative path of the file
    REL_PATH="${FILE_2#$TEMP_DIR_2/}"

    # Check if the file exists in the first repository
    FILE_1="$TEMP_DIR_1/$REL_PATH"

    if [ ! -f "$FILE_1" ]; then
        # Log files that do not exist in the first repository in red
        echo -e "${ERROR}File $FILE_1 does not exist in $TEMP_DIR_1${NC}"
        echo "File $FILE_1 does not exist in $TEMP_DIR_1" >> $LOGFILE
    fi
done

# NEW TEST - RUNNING SRCDIFF IN PARALLEL
# --------------------------------------
echo -e "${PARALLEL}NEW TEST - RUNNING SRCDIFF IN PARALLEL${NC}" >> $LOGFILE
echo "Collecting all .cpp and .h files to run srcDiff on all files at once" >> $LOGFILE

while true; do
    for i in {5..1}; do
        echo -ne "Loading Parallel Test in ${i}...  \r"
        sleep 1.0
    done
    break
done

# Start timer for total parallel execution time
PARALLEL_TOTAL_START_TIME=$SECONDS

# Collect all .cpp and .h files from both repositories
FILES_1=$(find "$TEMP_DIR_1" -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.c" \))
FILES_2=$(find "$TEMP_DIR_2" -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.c" \))

# Prepare a list of file pairs for parallel comparison
COMPARISON_LIST=$(mktemp)
for FILE_1 in $FILES_1; do
    # Get the relative path of the file
    REL_PATH="${FILE_1#$TEMP_DIR_1/}"

    # Find the corresponding file in the second repository
    FILE_2="$TEMP_DIR_2/$REL_PATH"
    if [ -f "$FILE_2" ]; then
        # Add the pair to the comparison list
        echo "$FILE_1 $FILE_2" >> $COMPARISON_LIST
    fi
done

# Function to run srcDiff on a pair of files
run_srcdiff_on_pair() {
    FILE_1=$1
    FILE_2=$2
    # Ensure branch names are displayed in the output
    echo -e "${PARALLEL}Comparing $(basename "$FILE_1") from $BRANCH_1 with $(basename "$FILE_2") from $BRANCH_2${NC}"
    echo -e "${PARALLEL}Comparing $(basename "$FILE_1") from $BRANCH_1 with $(basename "$FILE_2") from $BRANCH_2${NC}" >> $LOGFILE
    track_srcdiff_performance "srcdiff $FILE_1 $FILE_2 -o /dev/null"

    # Increment parallel comparison count
    ((PARALLEL_COMPARISON_COUNT++))
}

# Export the function and necessary variables for xargs
export -f run_srcdiff_on_pair
export -f track_srcdiff_performance
export TEMP_DIR_1
export TEMP_DIR_2
export BRANCH_1
export BRANCH_2
export LOGFILE

# Run srcDiff concurrently on all file pairs using xargs
cat $COMPARISON_LIST | xargs -n 2 -P 4 bash -c 'run_srcdiff_on_pair "$@"' _

# End timer for total parallel execution time
PARALLEL_TOTAL_END_TIME=$SECONDS
PARALLEL_TOTAL_TIME=$((PARALLEL_TOTAL_END_TIME - PARALLEL_TOTAL_START_TIME))

# Clean up temporary files and directories
rm -rf "$TEMP_DIR_1" "$TEMP_DIR_2" "$COMPARISON_LIST"

# Summary
echo "srcDiff stress test completed. Results saved to $LOGFILE."

# Print summary of file comparisons
echo "Summary of srcDiff comparisons:" >> $LOGFILE
echo "Sequential time to complete: $SEQUENTIAL_TOTAL_TIME seconds" >> $LOGFILE
echo "Parallel time to complete: $PARALLEL_TOTAL_TIME seconds" >> $LOGFILE


# Print summary to console
echo "Sequential time to complete: $SEQUENTIAL_TOTAL_TIME seconds"
echo "Parallel time to complete: $PARALLEL_TOTAL_TIME seconds"
echo "Total execution time: $((PARALLEL_TOTAL_TIME + SEQUENTIAL_TOTAL_TIME)) seconds"