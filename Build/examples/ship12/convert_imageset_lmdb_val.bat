G:
cd ../../x64/Release
convert_imageset --resize_height=256 --resize_width=256 --shuffle G:/Caffe-data/ship12/val/ G:/Caffe-data/ship12/val/val.txt G:/Caffe-data/ship12/ship12_val_lmdb
pause