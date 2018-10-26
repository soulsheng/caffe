G:
cd G:/ImageNet/bin/
convert_imageset --resize_height=256 --resize_width=256 --shuffle G:/Caffe-data/肠镜-病症信息库（6大类）/ G:/Caffe-data/肠镜-病症信息库（6大类）/train.txt G:/Caffe-data/ill6_train_lmdb
pause