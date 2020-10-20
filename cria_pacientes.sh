#!/bin/bash
file=/etc/passwd
if [ -f $file ] ; then
    cat $file | awk '/^a[0-9+]/' | head | awk -F':' '{print $1";"$5}' | awk -F';' '{print $1";"$2}' | awk -F';' '{print $1";"$2$3$4$5$1"@iscte-iul.pt"";""100"}' | sed 's/,/;/g' | sed 's/^a//' > pacientes.txt
else
    echo "O ficheiro não existe"
fi
