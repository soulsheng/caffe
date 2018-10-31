G:
cd ../../x64/Release
convert_imageset --resize_height=256 --resize_width=256 --shuffle G:/Caffe-data/re5/train/ G:/Caffe-data/re5/train/train.txt G:/Caffe-data/re5/re5_train_lmdb
pause