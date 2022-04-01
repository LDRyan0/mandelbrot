module load openmpi/2.1.2

CFLAGS="-lm -ansi -Wall"
FILES=()

REF=$(find . -type f -regex "\.\/0.*\.ppm")
CHECK=$(find . -type f -regex "\.\/[^0].*\.ppm")

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

echo "Checking all image files..."
# compile all programs
for f in $CHECK
do	
	var=$(diff $REF $f)
	if test -z "$var"
	then
		echo -e "${f##*/}: ${GREEN}PASSED${NC}"
	else
		echo -e "${f##*/}: ${RED}FAILED${NC}"
	fi
done






