F:
cd F:/ImageNet/bin/
convert_imageset --resize_height=256 --resize_width=256 --shuffle ../ILSVRC2012_img_train/ ./lable/train.txt G:/ImageNet/ilsvrc12_train_lmdb
pause