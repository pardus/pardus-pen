#!/bin/sh
cd data
genresource(){
    echo "<RCC>"
    echo "    <qresource prefix=\"/\">"
    {
        echo "tr.org.pardus.pen.default.conf"
        echo "tr.org.pardus.pen.svg"
        find images -type f
    } | sort | sed "s/^/        <file>/g;s/$/<\/file>/g"
    echo "    </qresource>"
    echo "</RCC>"
}

genresource > resources.qrc
