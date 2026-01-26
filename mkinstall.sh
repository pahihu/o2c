#
# install script
#
BINS="o2c o2b o2ef o2whereis system/o2gcc system/o2gcc.bat system/whis system/whis.bat"

if [ $# -ne 1 ];
then
    echo "usage: mkinstall.sh <install_dir>"
    exit 1
fi

DIR=$1
if [ -d $DIR -o -s $DIR ];
then
    echo "$DIR exists"
    exit 1
fi

if [ -s $HOME/.o2c.red ];
then
    echo "$HOME/.o2c.red exists"
    exit 1
fi

mkdir -p $DIR/bin
cp -rp compiler lib obj sym system $DIR
cp $BINS $DIR/bin

cat system/o2c.red.install | sed "s#%INSTALL_DIR%#$DIR#g" > $HOME/.o2c.red
sed -e "s#%INSTALL_DIR%#$DIR#g" -i "" $DIR/bin/whis
sed -e "s#%INSTALL_DIR%#$DIR#g" -i "" $DIR/bin/whis.bat
