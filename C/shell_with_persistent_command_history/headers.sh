#! /bin/sh

readonly AUTHOR_NAME="Sunit Singh"
readonly DATE=`date +%Y`
readonly COPYRIGHT="Copyright (C) $DATE $AUTHOR_NAME"

echo "Creating header files..."
ctags -x --c-kinds=f --file-scope=no *.c | awk -f headers.awk

for f in *.vs; do
    [ -f "$f" ] || break

    filename="$(basename $f .vs).h"

    echo -e "\tCreating $filename from $f"

    echo -e "/* $COPYRIGHT */\n" > $filename

    echo -e "/* Auto-generated */\n" >> $filename

    cat $f >> $filename
done

date > headers_timestamp
echo "Done creating headers."
