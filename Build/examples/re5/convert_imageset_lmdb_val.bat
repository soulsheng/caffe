G:
cd ../../x64/Release
convert_imageset --resize_height=256 --resize_width=256 --shuffle G:/Caffe-data/re5/test/ G:/Caffe-data/re5/test/val.txt G:/Caffe-data/re5/re5_val_lmdb
pause