G:
cd ../../x64/Release
convert_imageset --resize_height=256 --resize_width=256 --shuffle G:/Caffe-data/ship12/train/ G:/Caffe-data/ship12/train/train.txt G:/Caffe-data/ship12/ship12_train_lmdb
pause