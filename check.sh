# Validates output of each calculation method by checking .ppm file against
# .ppm file of the serial implementation

module load openmpi/2.1.2

CFLAGS="-lm -ansi -Wall"

# reference file stored in directory beggining with 0
REF=$(find . -type f -regex "\.\/0.*\.ppm")

# get all other ppm files 
CHECK=$(find . -type f -regex "\.\/[^0].*\.ppm")

# ANSI escape codes for text color
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

echo "Checking all .ppm image files..."

# check all programs using diff
for f in $CHECK
do	
	if test -z "$(diff $REF $f)" # no output from diff => match
	then
		echo -e "${f##*/}: ${GREEN}PASSED${NC}"
	else
		echo -e "${f##*/}: ${RED}FAILED${NC}"
	fi
done
