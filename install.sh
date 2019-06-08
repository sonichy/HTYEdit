s="[Desktop Entry]\nName=海天鹰编辑器\nComment=Text Editor\nExec=`pwd`/HTYEdit %u\nIcon=`pwd`/HTYEdit.png\nPath=`pwd`\nTerminal=false\nType=Application\nMimeType=text/plain;text/html;image/svg+xml;\nCategories=TextEditor;"
echo -e $s > HTYEdit.desktop
cp `pwd`/HTYEdit.desktop ~/.local/share/applications/HTYEdit.desktop