#!/bin/bash

if [[ $# -lt 5 ]]; then
    echo "stress_test.sh repo_1 branch_1 repo_2 branch_2"
fi

# Define Git repository URLs and (optionally) branches
REPO_1_URL=$1
BRANCH_1=$2

REPO_2_URL=$3
BRANCH_2=$4

# Create temporary directories for cloning
REPO_OLD=$(mktemp -d)
REPO_NEW=$(mktemp -d)

# Output log file
LOGFILE="srcdiff_git_comparison.log"

# Function to track srcDiff's performance
track_srcdiff_performance() {
    COMMAND="$1"
    echo "Running: $COMMAND" >> $LOGFILE
    /usr/bin/time $COMMAND 2>> $LOGFILE &
    SRC_DIFF_PID=$!

    # Monitor srcDiff performance using pidstat
    # echo "Tracking performance for srcDiff (PID: $SRC_DIFF_PID)" >> $LOGFILE
    # pidstat -p $SRC_DIFF_PID 1 >> $LOGFILE &
    # PIDSTAT_PID=$!

    # Wait for srcDiff to finish and then stop performance monitoring
    wait $SRC_DIFF_PID
    echo "Finished running srcDiff (PID: $SRC_DIFF_PID)" >> $LOGFILE
}

# Clone repositories
echo "Cloning repository $REPO_1_URL (branch $BRANCH_1) into $REPO_OLD" >> $LOGFILE
git clone --branch "$BRANCH_1" "$REPO_1_URL" "$REPO_OLD" || { echo "Error: Failed to clone $REPO_1_URL" >> $LOGFILE; exit 1; }

echo "Cloning repository $REPO_2_URL (branch $BRANCH_2) into $REPO_NEW" >> $LOGFILE
git clone --branch "$BRANCH_2" "$REPO_2_URL" "$REPO_NEW" || { echo "Error: Failed to clone $REPO_2_URL" >> $LOGFILE; exit 1; }

# Color codes for output messages
ERROR='\033[0;31m'      # Red for errors
REPORT='\033[0;34m'     # Green for parallel comparisons
NC='\033[0m'            # No color

# Run stress test
echo -e "${REPORT}Starting stress test...${NC}" >> $LOGFILE

track_srcdiff_performance "srcdiff $REPO_OLD $REPO_NEW -o temp.srcdiff.xml"
xmllint --noout temp.srcdiff.xml

# Clean up temporary files and directories
rm -rf "$REPO_OLD" "$REPO_NEW" temp.srcdiff.xml

# Print summary
echo "Summary:" >> $LOGFILE
