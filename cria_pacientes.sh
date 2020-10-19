#!/bin/bash
file=/etc/passwd
if [ -f $file ] ; then
    cat $file | grep "^a""[0-9+]" | head -10 | sed 's/^a//g' | cut -d':' -f1,5 | sed 's/:/;/g' | sed 's/,/;/g' > nomes.txt
    cat nomes.txt | cut -d';' -f1 | while read line; do
        cat nomes.txt | sed "s/$/a$line@iscte.iul.pt;100/" > pacientes.txt
    done
else
    echo "O ficheiro não existe"
fi
