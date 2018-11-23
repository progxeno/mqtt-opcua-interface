
username=$(whoami)
echo set IDF_PATH to system enironment variables
setx IDF_PATH C:/msys32/home/$username/esp/esp-idf //M

echo create idf_path.sh in /c/msys32/etc/profile.d/
echo 'export IDF_PATH="C:/msys32/home/'$username'/esp/esp-idf"' >/c/msys32/etc/profile.d/idf_path.sh